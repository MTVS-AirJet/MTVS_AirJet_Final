// Fill out your copyright notice in the Description page of Project Settings.


#include "LHJ/L_RoadTrigger.h"

#include "Components/BoxComponent.h"

// Sets default values
AL_RoadTrigger::AL_RoadTrigger()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RoadTriggerComp = CreateDefaultSubobject<UBoxComponent>(TEXT("RoadTriggerComp"));
	RootComponent = RoadTriggerComp;
}

// Called when the game starts or when spawned
void AL_RoadTrigger::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AL_RoadTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
