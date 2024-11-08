// Fill out your copyright notice in the Description page of Project Settings.


#include "JBS/J_ObjectiveTakeOff.h"
#include "JBS/J_BaseMissionObjective.h"
#include "JBS/J_Utility.h"
#include "JBS/J_MissionGamemode.h"
#include "Math/UnrealMathUtility.h"
#include "Kismet/GameplayStatics.h"
#include "KHS/K_CesiumTeleportBox.h"
#include "JBS/J_MissionPlayerController.h"
#include "TimerManager.h"


void AJ_ObjectiveTakeOff::ObjectiveActive()
{
    if(!HasAuthority()) return;

    GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("1. 이륙 시작"));
    
    // 모든 pc 가져오기
    allPC = UJ_Utility::GetAllMissionPC(GetWorld());
    allPawn = UJ_Utility::GetAllMissionPawn(GetWorld());

    // 맵 초기화
    for(auto* pc : allPC)
    {
        takeOffCheckMap.Add(pc, false);
    }

    // 게임모드 이륙 딜리게이트에 바인드
    auto* gm = UJ_Utility::GetMissionGamemode(GetWorld());
    gm->onePilotTakeOffDel.BindUObject(this, &AJ_ObjectiveTakeOff::SuccessTakeOff);
    // 미션 시작 딜리게이트 바인드
    gm->startTODel.BindUObject(this, &AJ_ObjectiveTakeOff::ObjectiveEnd);

    // 위치 텔포 박스로 설정
    auto* tpBox = Cast<AK_CesiumTeleportBox>(
        UGameplayStatics::GetActorOfClass(GetWorld(), AK_CesiumTeleportBox::StaticClass()));
    if(tpBox)
        SetPosition(tpBox);

    // 실패 체크 타이머 실행
    FTimerHandle timerHandle;
    GetWorld()->GetTimerManager()
        .SetTimer(checkTimeHandle, this, &AJ_ObjectiveTakeOff::CheckFail, 0.5f, true);

    Super::ObjectiveActive();
}

void AJ_ObjectiveTakeOff::SuccessTakeOff(AJ_MissionPlayerController *pc, bool isSuccess)
{
    if(!takeOffCheckMap.Contains(pc)) return;
    // 이륙 성공 처리
    takeOffCheckMap[pc] = isSuccess;

    // 현재 이륙 점수 정보 갱신
    CalcSuccessPercent();
}

void AJ_ObjectiveTakeOff::CalcSuccessPercent()
{
    // 모든 pc 데이터를 계산
    int cnt = 0;
    for(auto* pc : allPC)
    {
        // 성공 비율 계산
        if(!takeOffCheckMap.Contains(pc)) continue;
        // 이륙 성공 처리
        if(takeOffCheckMap[pc])
            cnt++;
    }

    // 평균 계산
    float rate = (float) cnt / allPC.Num();
    // 수행도에 적용
    this->SUCCESS_PERCENT = rate;
}

FTacticalOrderData AJ_ObjectiveTakeOff::SetObjUIData(class AJ_MissionPlayerController *pc)
{
    int maxCnt = allPC.Num();
    int curCnt = FMath::RoundToInt(this->SUCCESS_PERCENT * maxCnt);

    FTakeOffData data(curCnt, maxCnt);
    
    return FTacticalOrderData(this->orderType, data);
}

void AJ_ObjectiveTakeOff::Tick(float deltaTime)
{
    Super::Tick(deltaTime);
}

void AJ_ObjectiveTakeOff::SetPosition(class AK_CesiumTeleportBox *tpBox)
{
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
    // 모든 폰 검사
    for(auto* pc : allPC)
    {
        // 이륙 아직 아닐때 체크
        if(takeOffCheckMap[pc]) continue;

        // 목표를 넘어섰는데 true가 아닌거니깐 실패 처리
        auto* pawn = pc->GetPawn();
        // 전방 벡터
        auto myFV = this->GetActorForwardVector();
        auto pawnFV = pawn->GetActorForwardVector();

        // 내적
        float dot = FVector::DotProduct(myFV, pawnFV);
        
        // A 액터와 B 액터 간의 위치 벡터
        FVector toPawn = pawn->GetActorLocation() - this->GetActorLocation();
        toPawn.Normalize(); // 정규화

        // A가 B의 뒤쪽에 있는지 확인
        float dot2 = FVector::DotProduct(toPawn, pawnFV);
        bool isBehind = dot2 > 0;

        // 실패 거리만큼 떨어짐
        bool isFar = failDis < FVector::Dist(this->GetActorLocation(), pawn->GetActorLocation());
        
        // 실패 처리
        if (isBehind && isFar)
        {
            auto* gm = UJ_Utility::GetMissionGamemode(GetWorld());
            gm->AddFlightedPC(pc, false);
        }

        GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::Printf(TEXT("체크중 : pc : %s, 내적 %s, 거리 %s")
            , *pc->GetName(), *UJ_Utility::ToStringBool(isBehind), *UJ_Utility::ToStringBool(isFar)));
    }
}

void AJ_ObjectiveTakeOff::ObjectiveEnd(bool isSuccess)
{
    Super::ObjectiveEnd(isSuccess);

    // 타이머 핸들 해제
    GetWorld()->GetTimerManager().ClearTimer(checkTimeHandle);
}