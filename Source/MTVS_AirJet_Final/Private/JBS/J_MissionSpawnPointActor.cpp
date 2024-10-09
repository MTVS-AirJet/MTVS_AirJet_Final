// Fill out your copyright notice in the Description page of Project Settings.


#include "JBS/J_MissionSpawnPointActor.h"
#include "Components/ArrowComponent.h"

// Sets default values
AJ_MissionSpawnPointActor::AJ_MissionSpawnPointActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	rootComp = CreateDefaultSubobject<UArrowComponent>(TEXT("rootComp"));
	SetRootComponent(rootComp);
	
}

// Called when the game starts or when spawned
void AJ_MissionSpawnPointActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AJ_MissionSpawnPointActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

