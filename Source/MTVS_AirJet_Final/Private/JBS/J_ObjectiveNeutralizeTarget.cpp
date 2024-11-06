// Fill out your copyright notice in the Description page of Project Settings.


#include "JBS/J_ObjectiveNeutralizeTarget.h"
#include "DrawDebugHelpers.h"
#include "Engine/Engine.h"
#include "Engine/EngineTypes.h"
#include "Engine/HitResult.h"
#include "JBS/J_GroundTarget.h"
#include "JBS/J_Utility.h"
#include "Math/MathFwd.h"
#include <cfloat>
#include "JBS/J_MissionPlayerController.h"
#include "JBS/J_ObjectiveUIComp.h"
#include "TimerManager.h"

void AJ_ObjectiveNeutralizeTarget::BeginPlay()
{
    Super::BeginPlay();

    // 활성화시 지상 목표 생성 바인드
    objectiveActiveDel.AddUObject(this, &AJ_ObjectiveNeutralizeTarget::SpawnGroundTarget);
}

bool AJ_ObjectiveNeutralizeTarget::CalcSpawnTransform(FTransform& outSpawnTR)
{
    // 아래로 레이 쏴서 지면 포인트 찾기
    FHitResult outHit;
    ECollisionChannel traceChl = ECC_Visibility;
    FCollisionQueryParams params;
    params.AddIgnoredActor(this);
    // 히트 하는 위치 설정 
    FVector startLocation = this->GetActorLocation();
    FVector endLocation = startLocation + FVector::DownVector * MAX_FLT;
    bool isHit = GetWorld()->LineTraceSingleByChannel(
        outHit,
        startLocation,
        endLocation,
        traceChl,
        params
    );

    if(isHit)
    {
        // 위치
        FVector spLoc = outHit.ImpactPoint;
        // 회전
        // FRotator spRot = outHit.ImpactNormal.Rotation();
        FQuat spRot = FQuat::FindBetween(FVector::UpVector, outHit.ImpactNormal);
        
        // 스폰 트랜스폼 설정
        outSpawnTR = FTransform(spRot, spLoc, FVector::OneVector);
    }
    // @@ 그냥 지면 = -4km이라 가정
    else {
        FVector spLoc = this->GetActorLocation() + FVector::DownVector * UJ_Utility::defaultMissionObjectHeight;
        outSpawnTR = FTransform(FRotator::ZeroRotator, spLoc, FVector::OneVector);
    }

    return isHit;
}

// 활성화 딜리게이트에서 실행됨
void AJ_ObjectiveNeutralizeTarget::SpawnGroundTarget()
{
    // 스폰 위치 계산
    bool getSpawnTR = CalcSpawnTransform(spawnTR);
    if(!getSpawnTR)
    {
        GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("지상 목표 위치 대체 생성"));
        // FTimerHandle timerHandle;
        // GetWorld()->GetTimerManager()
        //     .SetTimer(timerHandle, [this]() mutable
        // {
        //     // 다시
        //     GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("다시 지상 목표 스폰 시도"));
        //     SpawnGroundTarget();
        // }, 0.1f, false);

        // return;
    }

    // @@ gt 캐시 해야 하려나?
    // @@ 여러개 소환하려면 분산시켜야 할 듯
    for(int i = 0; i < spawnTargetAmt; i++)
    {
        auto* groundTarget = GetWorld()->SpawnActor<AJ_GroundTarget>(groundTargetPrefab, spawnTR);
        GEngine->AddOnScreenDebugMessage(-1, 333.f, FColor::Green, FString::Printf(TEXT("스폰 위치 %s"), *spawnTR.ToString()));
        // 파괴 딜리게이트에 함수 넣기
        groundTarget->destroyedDelegate.AddUObject(this, &AJ_ObjectiveNeutralizeTarget::CountTargetDestroyed);
    }
}

void AJ_ObjectiveNeutralizeTarget::CountTargetDestroyed()
{
    destroyedTargetAmt++;

    // 수행도 갱신
    SUCCESS_PERCENT = static_cast<float>(destroyedTargetAmt) / spawnTargetAmt;

    // GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::White, FString::Printf(TEXT("현재 파괴된 수 : %d, 남은 수 : %d"), destroyedTargetAmt, (spawnTargetAmt - destroyedTargetAmt)));

    if(destroyedTargetAmt == spawnTargetAmt)
    {
        this->ObjectiveEnd(true);
    }
}

void AJ_ObjectiveNeutralizeTarget::Tick(float deltaTime)
{
    Super::Tick(deltaTime);

    // DrawDebugLine(
    //     GetWorld(),
    //     this->GetActorLocation(),
    //     GetActorLocation() + FVector::DownVector * FLT_MAX,
    //     FColor::Green,
    //     false,
    //     1.f,
    //     0,
    //     255.5f
    // );
}

void AJ_ObjectiveNeutralizeTarget::SRPC_StartNewObjUI()
{
    Super::SRPC_StartNewObjUI();

    // 보낼 데이터 
    auto orderData = SetObjUIData();

    
    // 모든 pc 가져오기
    auto allPC = UJ_Utility::GetAllMissionPC(GetWorld());

    // pc에게 새 전술명령 UI 시작 srpc
    for(auto* pc : allPC)
    {
        pc->objUIComp->CRPC_StartObjUI(orderData);
    }
}

void AJ_ObjectiveNeutralizeTarget::SRPC_UpdateObjUI()
{
    Super::SRPC_UpdateObjUI();

    // 보낼 데이터 
    auto orderData = SetObjUIData();
    
    // 모든 pc 가져오기
    auto allPC = UJ_Utility::GetAllMissionPC(GetWorld());

    // pc에게 새 전술명령 UI 시작 srpc
    for(auto* pc : allPC)
    {
        pc->objUIComp->CRPC_UpdateObjUI(orderData);
    }
}

void AJ_ObjectiveNeutralizeTarget::SRPC_EndObjUI()
{
    Super::SRPC_EndObjUI();
}

// void AJ_ObjectiveNeutralizeTarget::SRPC_EndSubObjUI()
// {
//     Super::SRPC_EndSubObjUI();
// }

FTacticalOrderData AJ_ObjectiveNeutralizeTarget::SetObjUIData(class AJ_MissionPlayerController *pc)
{
    FNeutralizeTargetUIData data(spawnTargetAmt, spawnTargetAmt - destroyedTargetAmt);

    return FTacticalOrderData(this->orderType, FFormationFlightUIData(), data);
}