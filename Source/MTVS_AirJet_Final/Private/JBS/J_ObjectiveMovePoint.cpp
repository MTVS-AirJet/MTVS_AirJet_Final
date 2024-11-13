// Fill out your copyright notice in the Description page of Project Settings.


#include "JBS/J_ObjectiveMovePoint.h"
#include "Components/CapsuleComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/Engine.h"
#include "Engine/HitResult.h"
#include "JBS/J_BaseMissionObjective.h"
#include "JBS/J_BaseMissionPawn.h"
#include "JBS/J_MissionPlayerController.h"
#include "JBS/J_Utility.h"
#include "Math/MathFwd.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "TimerManager.h"

AJ_ObjectiveMovePoint::AJ_ObjectiveMovePoint() : AJ_BaseMissionObjective()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    checkCapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("checkCapsuleComp"));
    checkCapsuleComp->SetupAttachment(rootComp);
    checkCapsuleComp->SetCapsuleHalfHeight(40000);
    checkCapsuleComp->SetCapsuleRadius(4000);
}

void AJ_ObjectiveMovePoint::BeginPlay()
{
    Super::BeginPlay();

    checkCapsuleComp->SetCapsuleHalfHeight(beamLength);
    checkCapsuleComp->SetCapsuleRadius(beamRadius);


    // 오버랩 바인드
    checkCapsuleComp->OnComponentBeginOverlap.AddDynamic( this, &AJ_ObjectiveMovePoint::OnCheckCapsuleBeginOverlap);

    objectiveActiveDel.AddUObject(this, &AJ_ObjectiveMovePoint::InitBeamVFX);
}

void AJ_ObjectiveMovePoint::InitBeamVFX()
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
    // FIXME 임시로 디버그 라인 생성
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
            4000.f
        );

        // GEngine->AddOnScreenDebugMessage(-1, .1f, FColor::White, TEXT("빔 생성 중"));
    }, 0.1, true);
}

void AJ_ObjectiveMovePoint::OnCheckCapsuleBeginOverlap(
    UPrimitiveComponent *OverlappedComponent,
    AActor *OtherActor, UPrimitiveComponent *OtherComp,
    int32 OtherBodyIndex, bool bFromSweep,
    const FHitResult &SweepResult)
{
    // 활성화 상태일때만
    if(!this->IS_OBJECTIVE_ACTIVE) return;

    // 충돌한게 미션 폰이면 목표 성공 처리
    if(OtherActor->IsA<AJ_BaseMissionPawn>())
    {
        // GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::White, FString::Printf(TEXT("충돌한 플레이어 : %s"), *OtherActor->GetName()));
        // GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::White, TEXT("이동 목표 성공"));
        // 수행도 1
        SUCCESS_PERCENT = 1.f;
        this->ObjectiveEnd(true);
    }   

    // GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::White, FString::Printf(TEXT("암튼 뭔가 충돌함 %s"), *OtherActor->GetName()));
}

void AJ_ObjectiveMovePoint::ObjectiveEnd(bool isSuccess)
{
    Super::ObjectiveEnd(isSuccess);
}

void AJ_ObjectiveMovePoint::SetObjectiveActive(bool value)
{
    Super::SetObjectiveActive(value);

    // 충돌체 활/비활성화
    checkCapsuleComp->SetActive(value);
    FName profile = value ? FName(TEXT("MovePoint")) : FName(TEXT("NoCollision"));
    checkCapsuleComp->SetCollisionProfileName(profile);

    if(!value)
    {
        // 빔 이펙트 종료
        // @@ VFx 로 변경하면 제거해야함
        GetWorld()->GetTimerManager().ClearTimer(timerHandle);
    }
}

void AJ_ObjectiveMovePoint::Tick(float deltaTime)
{
    Super::Tick(deltaTime);

    // GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::White, FString::Printf(TEXT("%d"), checkCapsuleComp->IsActive()));
    
    // 활성화 중 실패 체크
    if(!HasAuthority() || !IS_OBJECTIVE_ACTIVE) return;

    if(CheckFail(baseDirection))
        this->ObjectiveEnd(false);
    
}

void AJ_ObjectiveMovePoint::ObjectiveActive()
{
    Super::ObjectiveActive();
    AJ_MissionPlayerController* localPC;
    UJ_Utility::GetLocalPlayerController(GetWorld(), localPC);
    
    getp = localPC->GetPawn<AJ_BaseMissionPawn>();

    getworldPawn = Cast<AJ_BaseMissionPawn>(
        UGameplayStatics::GetActorOfClass(GetWorld(), AJ_BaseMissionPawn::StaticClass()));
    if(getworldPawn)
    {
        //해당 액터로 뭔가 하기
        GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString::Printf(TEXT("%s"), *getworldPawn->GetName()));
    }
    // 편대장 위치 가져오기
    auto* localPawn = UJ_Utility::GetBaseMissionPawn(GetWorld());
    const auto& leaderLoc = localPawn->GetActorLocation();
    // 기준 방향 구하기
    baseDirection = (this->GetActorLocation() - leaderLoc).GetSafeNormal();
}

bool AJ_ObjectiveMovePoint::CheckFail(const FVector &baseDir)
{
    // 모든 파일럿 중 한 명이라도 기준 방향을 넘어가면 실패 처리
    const auto& allPilot = UJ_Utility::GetAllMissionPawn(GetWorld());
    
    for(const auto* pilot : allPilot)
    {
        // 현재 방향
        const auto& curDir = (this->GetActorLocation() - pilot->GetActorLocation()).GetSafeNormal();

        // 내적
        float check = FVector::DotProduct(baseDir, curDir);
        // 거리
        float dis = FVector::Dist(this->GetActorLocation(), pilot->GetActorLocation());

        // GEngine->AddOnScreenDebugMessage(-1, -1.f, FColor::Green, FString::Printf(TEXT("actor : %s\n내적 중  : %.2f, 거리 : %.2f")
        // , *this->GetName()
        // , check
        // , dis));

        // 실패
        if(check < 0 && dis > failDis)
            return true;
    }

    return false;
}