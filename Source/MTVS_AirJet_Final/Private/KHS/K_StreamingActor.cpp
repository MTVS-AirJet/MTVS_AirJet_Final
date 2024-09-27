// Fill out your copyright notice in the Description page of Project Settings.


#include "KHS/K_StreamingActor.h"

// Sets default values
AK_StreamingActor::AK_StreamingActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AK_StreamingActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AK_StreamingActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

