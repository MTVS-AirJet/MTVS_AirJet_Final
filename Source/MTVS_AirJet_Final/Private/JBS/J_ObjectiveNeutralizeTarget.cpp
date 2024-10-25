// Fill out your copyright notice in the Description page of Project Settings.


#include "JBS/J_ObjectiveNeutralizeTarget.h"
#include "DrawDebugHelpers.h"
#include "Engine/Engine.h"
#include "Engine/EngineTypes.h"
#include "Engine/HitResult.h"
#include "JBS/J_GroundTarget.h"
#include "Math/MathFwd.h"
#include <cfloat>

void AJ_ObjectiveNeutralizeTarget::BeginPlay()
{
    Super::BeginPlay();

    objectiveActiveDel.AddUObject(this, &AJ_ObjectiveNeutralizeTarget::SpawnGroundTarget);
}

FTransform AJ_ObjectiveNeutralizeTarget::CalcSpawnTransform()
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

    FTransform result;

    if(isHit)
    {
        // 위치
        FVector spLoc = outHit.ImpactPoint;
        // 회전
        // FRotator spRot = outHit.ImpactNormal.Rotation();
        FQuat spRot = FQuat::FindBetween(FVector::UpVector, outHit.ImpactNormal);
        
        // 스폰 트랜스폼 설정
        result = FTransform(spRot, spLoc, FVector::OneVector);
    }
    else {
        GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("바닥 없어 지상 타겟 생성 못함"));
    }

    return result;
}

// 활성화 딜리게이트에서 실행됨
void AJ_ObjectiveNeutralizeTarget::SpawnGroundTarget()
{
    // 스폰 위치 계산
    spawnTR = CalcSpawnTransform();

    // 알고있어야 하려나?
    for(int i = 0; i < spawnTargetAmt; i++)
    {
        auto* groundTarget = GetWorld()->SpawnActor<AJ_GroundTarget>(groundTargetPrefab, spawnTR);
        // 파괴 딜리게이트에 함수 넣기
        groundTarget->destroyedDelegate.AddUObject(this, &AJ_ObjectiveNeutralizeTarget::CountTargetDestroyed);
    }
}

void AJ_ObjectiveNeutralizeTarget::CountTargetDestroyed()
{
    destroyedTargetAmt++;

    // 수행도 갱신
    SUCCESS_PERCENT = static_cast<float>(destroyedTargetAmt) / spawnTargetAmt;

    GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::White, FString::Printf(TEXT("현재 파괴된 수 : %d, 남은 수 : %d"), destroyedTargetAmt, (spawnTargetAmt - destroyedTargetAmt)));

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
    //     GetActorLocation() + FVector(0,0,-1) * FLT_MAX,
    //     FColor::Green,
    //     false,
    //     -1,
    //     0,
    //     5.5f
    // );
}
