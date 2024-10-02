// Fill out your copyright notice in the Description page of Project Settings.


#include "LHJ/Viper.h"

// Sets default values
AViper::AViper()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AViper::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AViper::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AViper::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

