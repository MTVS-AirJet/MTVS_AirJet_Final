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
    auto allPC = UJ_Utility::GetAllMissionPC(GetWorld());
    auto allPawn = UJ_Utility::GetAllMissionPawn(GetWorld());

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

    // XXX ai 시동 마지막에서 하는중
    // ReqPlayCommVoice(10, allPC);
}

void AJ_ObjectiveTakeOff::SetPosition(class AK_CesiumTeleportBox *tpBox)
{
    if(!HasAuthority()) return;
    if(!tpBox) return;

    auto* localPC = GetWorld()->GetFirstPlayerController();
    if(!localPC) return;
    
    auto* localPawn = localPC->GetPawn();
    if(!localPawn) return;
    
    // 위치 텔포 박스로
    this->SetActorLocation(tpBox->GetActorLocation());
    // 편대장 바라보게 회전
    FVector direction = localPawn->GetActorLocation() - this->GetActorLocation();
    direction.Normalize(); // 방향 벡터를 정규화

    FRotator newRotation = FRotationMatrix::MakeFromX(direction).Rotator(); // 방향 벡터로부터 로테이션 생성
    this->SetActorRotation(newRotation); // 액터의 로테이션 설정
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

#pragma region 이륙 단
/* 한 명 이륙 시 이륙 성공 딜리게이트 실행 | SuccessTakeOff
-> 이륙 점수 갱신 | CalcSuccessPercent
-> 전부 이륙시 매니저 쪽에서 완료 처리 | ObjectiveEnd*/
void AJ_ObjectiveTakeOff::SuccessTakeOff(AJ_MissionPlayerController *pc, bool isSuccess)
{
    if(!HasAuthority()) return;

    if(!takeOffCheckMap.Contains(pc)) return;
    // 이륙 성공 처리
    takeOffCheckMap[pc] = FTakeOffCheckData(true, isSuccess);
    
    // 현재 이륙 점수 정보 갱신
    CalcSuccessPercent();
}

void AJ_ObjectiveTakeOff::CalcSuccessPercent()
{
    if(!HasAuthority()) return;

    // 모든 pc 데이터를 계산
    auto allPC = UJ_Utility::GetAllMissionPC(GetWorld());
    
    int maxCnt = allPC.Num();
    int cnt = 0;
    int curFlightCnt = 0;
    for(auto* pc : allPC)
    {
        if(!pc) continue;
        if(!takeOffCheckMap.Contains(pc)) continue;


        // 성공 비율 계산
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

void AJ_ObjectiveTakeOff::ObjectiveEnd(bool isSuccess)
{
    Super::ObjectiveEnd(isSuccess);

    // 타이머 핸들 해제
    GetWorld()->GetTimerManager().ClearTimer(checkTimeHandle);
}

#pragma endregion

#pragma region 반복 단
/* begin에서 실패 판정 타이머 반복 | CheckFail
->
*/
void AJ_ObjectiveTakeOff::Tick(float deltaTime)
{
    Super::Tick(deltaTime);

    if(!HasAuthority()) return;
}

void AJ_ObjectiveTakeOff::CheckFail()
{
    if(!HasAuthority()) return;
    auto allPC = UJ_Utility::GetAllMissionPC(GetWorld());

    // 모든 폰 검사
    for(auto* pc : allPC)
    {
        // pc 없으면 통과
        if(!pc) continue;
        if(!takeOffCheckMap.Contains(pc)) continue;
        // 이륙 아직 아닐때 체크
        if(takeOffCheckMap[pc].isTakeOff) continue;

        auto* pilot = pc->GetPawn();
        if(!pilot) continue;


        // 현재 방향
        const auto& curDir = (this->GetActorLocation() - pilot->GetActorLocation()).GetSafeNormal();
        // 내적
        float check = FVector::DotProduct(baseDirection, curDir);
        // 거리
        float dis = FVector::Dist(this->GetActorLocation(), pilot->GetActorLocation());

        // 목표를 넘어섰는데 true가 아닌거니깐 실패 처리
        if(check < 0 && dis > failDis)
        {
            auto* gm = UJ_Utility::GetMissionGamemode(GetWorld());
            gm->AddFlightedPC(pc, false);
        }
    }
}

#pragma endregion

#pragma region 목표 UI 설정 단
/* 특정 타이밍에 목표 UI 시작 | StartNewObjUI
-> 각 pc마다 목표 ui 데이터 설정 | SendObjUIData, SetTakeOffUIData
-> 특정 갱신 타이밍 마다 업데이트 UI | UpdateObjUI -> 이전에 보냈는지 체크후 업데이트 | CheckSendSameData
-> 목표 종료 나 서브 목표 종료시 ui 애니메이션 실행 요청 | EndObjUI , EndSubObjUI
-> 목표 종료시 비작동*/
void AJ_ObjectiveTakeOff::SendObjUIData(class AJ_MissionPlayerController *pc, bool isInit)
{
    Super::SendObjUIData(pc, isInit);

    auto data = SetTakeOffUIData(pc);
    pc->objUIComp->CRPC_StartObjUITakeOff(data);
}

FTakeOffData AJ_ObjectiveTakeOff::SetTakeOffUIData(class AJ_MissionPlayerController *pc)
{
    if(!HasAuthority()) return FTakeOffData();
    auto allPC = UJ_Utility::GetAllMissionPC(GetWorld());

    int maxCnt = allPC.Num();
    int curCnt = FMath::RoundToInt(curFlightPercent);

    return FTakeOffData(curCnt, maxCnt);
}

void AJ_ObjectiveTakeOff::UpdateObjUI()
{
    if(!HasAuthority()) return;

    // 모든 pc 가져오기
    auto allPC = UJ_Utility::GetAllMissionPC(GetWorld());
    // pc에게 새 전술명령 UI 시작 srpc
    for(auto* pc : allPC)
    {
		// 과도한 crpc 방지 처리
		// 이전에 보낸 ui 데이터와 동일한지 검증
        auto orderData = SetTakeOffUIData(pc);

		// 다르면 업데이트
		// @@ 템플릿 쓸 수 있을지 고민
		FTacticalOrderData temp(this->orderType, orderData);
		bool canUpdate = CheckSendSameData(pc, temp);
		if(canUpdate)
		{
			// 데이터 보내기
			pc->objUIComp->CRPC_UpdateObjUITakeOff(orderData);
		}
    }
}

#pragma endregion





