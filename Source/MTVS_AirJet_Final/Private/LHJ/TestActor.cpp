// Fill out your copyright notice in the Description page of Project Settings.


#include "LHJ/TestActor.h"

#include "MTVS_AirJet_Final/MTVS_AirJet_Final.h"

// Sets default values
ATestActor::ATestActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ATestActor::BeginPlay()
{
	Super::BeginPlay();
	LOG_S(Warning, TEXT("Log_Warning"));
	LOG_SCREEN("Log_Screen");
	LOG_SCREEN_T(3.f, "Log_Screen_T");
}

// Called every frame
void ATestActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
