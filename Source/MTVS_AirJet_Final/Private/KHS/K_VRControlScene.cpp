// Fill out your copyright notice in the Description page of Project Settings.


#include "KHS/K_VRControlScene.h"

// Sets default values for this component's properties
UK_VRControlScene::UK_VRControlScene()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UK_VRControlScene::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UK_VRControlScene::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

