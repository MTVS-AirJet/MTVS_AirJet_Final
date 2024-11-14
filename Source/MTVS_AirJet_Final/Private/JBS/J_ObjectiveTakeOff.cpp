// Fill out your copyright notice in the Description page of Project Settings.


#include "JBS/J_ObjectiveTakeOff.h"
#include "Engine/Engine.h"
#include "JBS/J_BaseMissionObjective.h"
#include "JBS/J_BaseMissionPawn.h"
#include "JBS/J_Utility.h"
#include "JBS/J_MissionGamemode.h"
#include "Math/UnrealMathUtility.h"
#include "Kismet/GameplayStatics.h"
#include "KHS/K_CesiumTeleportBox.h"
#include "JBS/J_MissionPlayerController.h"
#include "TimerManager.h"
#include "JBS/J_ObjectiveUIComp.h"

#pragma region 시작 단
/* active -> 게임모드 이륙시 딜리게이트에 이륙 체크 함수 바인드 | AddFlightedPC -> SuccessTakeOff
-> 목표 위치 텔포 박스로 설정 | SetPosition
-> 실패 체크 기준 방향 설정 | CalcBaseDirection*/
void AJ_ObjectiveTakeOff::BeginPlay()
{
    Super::BeginPlay();

    if(!HasAuthority()) return;
}

void AJ_ObjectiveTakeOff::ObjectiveActive()
{
    Super::ObjectiveActive();

    // GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("1. 이륙 시작"));
    
    // 모든 pc 가져오기
    auto& allPC = UJ_Utility::GetAllMissionPC(GetWorld());
    auto& allPawn = UJ_Utility::GetAllMissionPawn(GetWorld());

    // 맵 초기화
    takeOffCheckMap.Empty();
    for(auto* pc : allPC)
    {
        takeOffCheckMap.Add(pc, FTakeOffCheckData(false, false));
    }

    // 게임모드 이륙 딜리게이트에 바인드
    AJ_MissionGamemode* gm = UJ_Utility::GetMissionGamemode(GetWorld());
    gm->onePilotTakeOffDel.BindUObject(this, &AJ_ObjectiveTakeOff::SuccessTakeOff);
    
    // 위치 텔포 박스로 설정
    if(gm->TP_BOX)
        SetPosition(gm->TP_BOX);

    // 실패 체크 기준 방향 설정
    baseDirection = CalcBaseDirection();

    // FIXME 살리기
    // 실패 체크 타이머 실행
    // GetWorld()->GetTimerManager()
    //     .SetTimer(checkTimeHandle, this, &AJ_ObjectiveTakeOff::CheckFail, failCheckInterval, true);

    StartNewObjUI();

    // ai
    ReqPlayCommVoice(10, allPC);
}

FVector AJ_ObjectiveTakeOff::CalcBaseDirection()
{   
    auto* localPC = GetWorld()->GetFirstPlayerController();
    if(!localPC) return this->GetActorForwardVector();

    auto* localPawn = localPC->GetPawn();
    if(!localPawn) return this->GetActorForwardVector();

    const auto& leaderLoc = localPawn->GetActorLocation();
    // 기준 방향 구하기
    return (this->GetActorLocation() - leaderLoc).GetSafeNormal();
}

#pragma endregion

// FIXME 여기부터 리팩토링 시작



void AJ_ObjectiveTakeOff::SuccessTakeOff(AJ_MissionPlayerController *pc, bool isSuccess)
{
    if(!HasAuthority()) return;

    if(!takeOffCheckMap.Contains(pc)) return;
    // 이륙 성공 처리
    takeOffCheckMap[pc] = FTakeOffCheckData(true, isSuccess);
    
    // 현재 이륙 점수 정보 갱신
    CalcSuccessPercent();

    ObjectiveEnd(true);

}

void AJ_ObjectiveTakeOff::CalcSuccessPercent()
{
    if(!HasAuthority()) return;
    // 모든 pc 데이터를 계산
    int maxCnt = allPC.Num();
    int cnt = 0;
    int curFlightCnt = 0;
    for(auto* pc : allPC)
    {
        // 성공 비율 계산
        if(!takeOffCheckMap.Contains(pc)) continue;
        // 이륙 성공 처리
        if(takeOffCheckMap[pc].isSuccess)
            cnt++;
        if(takeOffCheckMap[pc].isTakeOff)
            curFlightCnt++;
    }

    // 평균 계산
    float rate = (float) cnt / maxCnt;
    curFlightPercent = (float) curFlightCnt / maxCnt;
    // 수행도에 적용
    this->SUCCESS_PERCENT = rate;
}

FTacticalOrderData AJ_ObjectiveTakeOff::SetObjUIData(class AJ_MissionPlayerController *pc)
{
    if(!HasAuthority()) return FTacticalOrderData();
    int maxCnt = allPC.Num();
    int curCnt = FMath::RoundToInt(curFlightPercent);

    FTakeOffData data(curCnt, maxCnt);
    
    return FTacticalOrderData(this->orderType, data);
}

void AJ_ObjectiveTakeOff::Tick(float deltaTime)
{
    Super::Tick(deltaTime);

    if(!HasAuthority()) return;
}

void AJ_ObjectiveTakeOff::SetPosition(class AK_CesiumTeleportBox *tpBox)
{
    if(!HasAuthority()) return;
    // 위치 텔포 박스로
    this->SetActorLocation(tpBox->GetActorLocation());
    // 편대장 바라보게 회전
    auto* host = allPC[0]->GetPawn();
    FVector direction = host->GetActorLocation() - this->GetActorLocation();
    direction.Normalize(); // 방향 벡터를 정규화
    FRotator newRotation = FRotationMatrix::MakeFromX(direction).Rotator(); // 방향 벡터로부터 로테이션 생성
    this->SetActorRotation(newRotation); // 액터의 로테이션 설정
}

void AJ_ObjectiveTakeOff::CheckFail()
{
    if(!HasAuthority()) return;
    auto allPC2 = UJ_Utility::GetAllMissionPC(GetWorld());

    // 모든 폰 검사
    for(auto* pc : allPC2)
    {
        // pc 없으면 통과
        if(!pc) continue;

        // 이륙 아직 아닐때 체크
        if(takeOffCheckMap[pc].isTakeOff) continue;

        // 목표를 넘어섰는데 true가 아닌거니깐 실패 처리
        auto* pilot = pc->GetPawn();

        if(!pilot) continue;

        // 현재 방향
        const auto& curDir = (this->GetActorLocation() - pilot->GetActorLocation()).GetSafeNormal();

        // 내적
        float check = FVector::DotProduct(baseDirection, curDir);
        // 거리
        float dis = FVector::Dist(this->GetActorLocation(), pilot->GetActorLocation());
        // GEngine->AddOnScreenDebugMessage(-1, .5f, FColor::Green, FString::Printf(TEXT("actor : %s\n내적 중  : %.2f, 거리 : %.2f")
        // , *this->GetName()
        // , check
        // , dis));
        if(check < 0 && dis > failDis)
        {
            auto* gm = UJ_Utility::GetMissionGamemode(GetWorld());
            gm->AddFlightedPC(pc, false);

            continue;
        }
    }
}

void AJ_ObjectiveTakeOff::ObjectiveEnd(bool isSuccess)
{
    Super::ObjectiveEnd(isSuccess);

    if(!HasAuthority()) return;
    // 타이머 핸들 해제
    GetWorld()->GetTimerManager().ClearTimer(checkTimeHandle);
}

void AJ_ObjectiveTakeOff::SRPC_StartNewObjUI()
{
    Super::SRPC_StartNewObjUI();
}

void AJ_ObjectiveTakeOff::StartNewObjUI()
{
    if(!HasAuthority() || !IS_OBJECTIVE_ACTIVE) return;

	// 모든 pc 가져오기
	auto allPC2 = UJ_Utility::GetAllMissionPC(GetWorld());

    // pc에게 새 전술명령 UI 시작 crpc
    for(auto* pc : allPC2)
    {
		// 보낼 목표 데이터 구성
		auto orderData = SetTakeOffUIData(pc);
		// ui 생성 시작
        pc->objUIComp->CRPC_StartObjUITakeOff(orderData);
    }
}

void AJ_ObjectiveTakeOff::SRPC_UpdateObjUI()
{
    Super::SRPC_UpdateObjUI();

    if(!HasAuthority() || !IS_OBJECTIVE_ACTIVE) return;

	
}

FTakeOffData AJ_ObjectiveTakeOff::SetTakeOffUIData(class AJ_MissionPlayerController *pc)
{
    if(!HasAuthority()) return FTakeOffData();
    auto allPC2 = UJ_Utility::GetAllMissionPC(GetWorld());

    int maxCnt = allPC2.Num();
    int curCnt = FMath::RoundToInt(curFlightPercent);

    return FTakeOffData(curCnt, maxCnt);
}

void AJ_ObjectiveTakeOff::UpdateObjUI()
{
    // 보낼 데이터
    // 모든 pc 가져오기
    auto allPC2 = UJ_Utility::GetAllMissionPC(GetWorld());

    // pc에게 새 전술명령 UI 시작 srpc
    for(auto* pc : allPC2)
    {
		auto orderData = SetTakeOffUIData(pc);
		
        FTacticalOrderData tempData(this->orderType, orderData);

		// 과도한 crpc 방지 처리
		if(!prevObjUIDataMap.Contains(pc) || tempData != prevObjUIDataMap[pc])
		{
			// 데이터 보내기
			pc->objUIComp->CRPC_UpdateObjUITakeOff(orderData);
			// objui데이터 맵에 저장
			prevObjUIDataMap.Add(pc, tempData);
		}
    }
}
