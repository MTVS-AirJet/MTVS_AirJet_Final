// Fill out your copyright notice in the Description page of Project Settings.


#include "JBS/J_CommanderPlayer.h"
#include "Engine/LocalPlayer.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/PlayerController.h"
#include "JBS/J_GroundTarget.h"
#include "JBS/J_MissionActorInterface.h"
#include "JBS/J_TacticalViewMonitor.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AJ_CommanderPlayer::AJ_CommanderPlayer()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
// 즉시 포제스되어있지 않기때문에 pc 사용 불가
void AJ_CommanderPlayer::BeginPlay()
{
	Super::BeginPlay();
}

// 사실상 이게 beginplay 취급
void AJ_CommanderPlayer::PossessedBy(AController *newController)
{	
	Super::PossessedBy(newController);
	
	auto* pc = Cast<APlayerController>(newController);
	if(pc)
	{
		auto* subSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(pc->GetLocalPlayer());
		if(subSystem)
			subSystem->AddMappingContext(imcCommander, 0);
	}

	// 전술 모니터 가져오기
	auto* tempTacViewMonitor = Cast<AJ_TacticalViewMonitor>(
		UGameplayStatics::GetActorOfClass(GetWorld(), AJ_TacticalViewMonitor::StaticClass()));
	if(tempTacViewMonitor)
	{
		//해당 액터로 뭔가 하기
		tacViewMonitor = tempTacViewMonitor;
	}
}

// Called every frame
void AJ_CommanderPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AJ_CommanderPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

// value = 스크롤 세기 및 방향 +-
// value negate 처리 되어있음
void AJ_CommanderPlayer::EventScroll(float value)
{
	SRPC_MapScroll(value);
}

void AJ_CommanderPlayer::SRPC_MapScroll_Implementation(float value)
{
	// 값 * 감도
	float result = value*mapScrollSensitivity;
	// 전술 모니터에 전달
	tacViewMonitor->SetMapFov(result);
}