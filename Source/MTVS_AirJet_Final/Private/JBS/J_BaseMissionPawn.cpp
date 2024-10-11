// Fill out your copyright notice in the Description page of Project Settings.


#include "JBS/J_BaseMissionPawn.h"
#include "KHS/K_GameState.h"
#include <JBS/J_Utility.h>

// Sets default values
AJ_BaseMissionPawn::AJ_BaseMissionPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AJ_BaseMissionPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AJ_BaseMissionPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AJ_BaseMissionPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AJ_BaseMissionPawn::ServerRPC_SetStreamingPlayer_Implementation(const FString &playerId, bool bAddPlayer)
{
	// k 게임 스테이트 가져오기
	auto* gs = UJ_Utility::GetKGameState(GetWorld());
	// userId 배열에 추가 or 제거
	if(bAddPlayer)
	{
		if(gs->ArrStreamingUserID.Find(playerId) >= 0)
			return;

		gs->ArrStreamingUserID.Add(playerId);
	}
	else
	{
		if(gs->ArrStreamingUserID.Num() == 0 
		|| gs->ArrStreamingUserID.Find(playerId) < 0 )
			return;

		gs->ArrStreamingUserID.Remove(playerId);
	}
	// 동기화
	if(this->HasAuthority())
		gs->OnRep_StreamingID();
}