// Fill out your copyright notice in the Description page of Project Settings.


#include "JBS/J_GroundTarget.h"

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

