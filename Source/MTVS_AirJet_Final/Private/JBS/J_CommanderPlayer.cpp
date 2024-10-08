// Fill out your copyright notice in the Description page of Project Settings.


#include "JBS/J_CommanderPlayer.h"

// Sets default values
AJ_CommanderPlayer::AJ_CommanderPlayer()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AJ_CommanderPlayer::BeginPlay()
{
	Super::BeginPlay();
	
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

