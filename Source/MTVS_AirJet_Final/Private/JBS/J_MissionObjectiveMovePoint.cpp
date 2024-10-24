// Fill out your copyright notice in the Description page of Project Settings.


#include "JBS/J_MissionObjectiveMovePoint.h"
#include "Components/CapsuleComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/HitResult.h"
#include "JBS/J_BaseMissionObjective.h"
#include "JBS/J_BaseMissionPawn.h"
#include "Math/MathFwd.h"
#include "NiagaraFunctionLibrary.h"
#include "TimerManager.h"

AJ_MissionObjectiveMovePoint::AJ_MissionObjectiveMovePoint() : AJ_BaseMissionObjective()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    checkCapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("checkCapsuleComp"));
    checkCapsuleComp->SetupAttachment(rootComp);
    checkCapsuleComp->SetCapsuleHalfHeight(40000);
}

void AJ_MissionObjectiveMovePoint::BeginPlay()
{
    Super::BeginPlay();

    // 오버랩 바인드
    checkCapsuleComp->OnComponentBeginOverlap.AddDynamic( this, &AJ_MissionObjectiveMovePoint::OnCheckCapsuleBeginOverlap);

    objectiveActiveDel.AddUObject(this, &AJ_MissionObjectiveMovePoint::InitBeamVFX);
}

void AJ_MissionObjectiveMovePoint::InitBeamVFX()
{
    // 내 위치 아래에 ray 쏴서 바닥 가져오기
    FHitResult outHit;
    ECollisionChannel traceChl = ECC_Visibility;
    FCollisionQueryParams params;
    params.AddIgnoredActor(this);
    // 히트 하는 위치 설정 
    FVector startLocation = this->GetActorLocation();
    FVector endLocation = startLocation + FVector::DownVector * beamLength / 2;
    bool isHit = GetWorld()->LineTraceSingleByChannel(
        outHit,
        startLocation,
        endLocation,
        traceChl,
        params
    );
    
    // 충돌 지점 기준 정점 정하기
    // 시작점
    FVector beamStartLoc;
    // 끝점
    FVector beamEndLoc;
    if(isHit)
    {
        beamStartLoc = outHit.ImpactPoint;
        beamEndLoc = beamStartLoc + FVector::UpVector * beamLength;
    }
    else {
        beamStartLoc = endLocation;
        beamEndLoc = beamStartLoc + FVector::UpVector * beamLength;
    }

    // 해당 포인트에서 상공 아주 높게 빔 생성
    // @@ 임시로 디버그 라인 생성
    GetWorld()->GetTimerManager()
        .SetTimer(timerHandle, [this,beamStartLoc,beamEndLoc]() mutable
    {
        DrawDebugLine(
            GetWorld(),
            beamStartLoc,
            beamEndLoc,
            FColor::Blue,
            false,
            .1f,
            0,
            250.5f
        );
    }, 0.1, true);

    GEngine->AddOnScreenDebugMessage(-1, 22, FColor::Green, FString::Printf(TEXT("빔 생성 %s"), isHit ? TEXT("성공") : TEXT("실패")));
}

void AJ_MissionObjectiveMovePoint::OnCheckCapsuleBeginOverlap(
    UPrimitiveComponent *OverlappedComponent,
    AActor *OtherActor, UPrimitiveComponent *OtherComp,
    int32 OtherBodyIndex, bool bFromSweep,
    const FHitResult &SweepResult)
{
    // 충돌한게 미션 폰이면 목표 성공 처리
    if(OtherActor->IsA<AJ_BaseMissionPawn>())
    {
        GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::White, TEXT("이동 목표 성공"));
        
        this->ObjectiveEnd(true);
    }   
}

void AJ_MissionObjectiveMovePoint::ObjectiveEnd(bool isSuccess)
{
    Super::ObjectiveEnd(isSuccess);
}

void AJ_MissionObjectiveMovePoint::SetObjectiveActive(bool value)
{
    Super::SetObjectiveActive(value);

    // 충돌체 활/비활성화
    checkCapsuleComp->SetActive(value);
    if(!value)
    {
        // 빔 이펙트 종료
        // @@
        GetWorld()->GetTimerManager().ClearTimer(timerHandle);
    }
}