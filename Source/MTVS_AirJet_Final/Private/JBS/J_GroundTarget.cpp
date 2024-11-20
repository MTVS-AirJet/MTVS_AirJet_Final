// Fill out your copyright notice in the Description page of Project Settings.


#include "JBS/J_GroundTarget.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "JBS/J_BaseMissionPawn.h"
#include "JBS/J_MissionPlayerController.h"
#include "JBS/J_Utility.h"
#include "Templates/Casts.h"
#include "TimerManager.h"

// Sets default values
AJ_GroundTarget::AJ_GroundTarget()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AJ_GroundTarget::BeginPlay()
{
	Super::BeginPlay();
	
	if(debugDestroySelf)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("테스트용 자괴 타이머 시작"));
		
		FTimerHandle timerHandle;
		GetWorld()->GetTimerManager()
			.SetTimer(timerHandle, [this]() mutable
		{
			//타이머에서 할 거
			Death();
		}, 3.f, false);

	}
}

// Called every frame
void AJ_GroundTarget::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AJ_GroundTarget::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AJ_GroundTarget::GetDamage(AActor *attacker, FVector hitPoint, FVector hitNormal)
{
	// FIXME 맞은 포인트로 점수 계산 필요

	// 공격한 플레이어 가져오기
	const auto* pilot = CastChecked<AJ_BaseMissionPawn>(attacker);
	auto* hitPC = pilot->GetController<AJ_MissionPlayerController>();
	check(hitPC);

	// 점수 보내기
	sendScoreDel.Broadcast(hitPC, 1.0f);

	// Death();
}

void AJ_GroundTarget::Death()
{
	// 사망 딜리게이트 실행
	destroyedDelegate.Broadcast();

	// 파괴
	this->Destroy();
}

void AJ_GroundTarget::SetTargetActive(bool value)
{
	// 안보이게 
	this->SetActorHiddenInGame(!value);

	// 충돌 판정 처리
	auto* meshComp = this->GetComponentByClass<UStaticMeshComponent>();
	FName collisionName = value ? FName(TEXT("GroundTarget")) : FName(TEXT("NoCollision"));
	meshComp->SetCollisionProfileName(collisionName);
}