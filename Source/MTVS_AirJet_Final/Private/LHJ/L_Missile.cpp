// Fill out your copyright notice in the Description page of Project Settings.


#include "LHJ/L_Missile.h"

#include "Components/BoxComponent.h"

// Sets default values
AL_Missile::AL_Missile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MissileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MissileMesh"));
	SetRootComponent(MissileMesh);

	MissileBoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("MissileBoxComp"));
	MissileBoxComp->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AL_Missile::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AL_Missile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
