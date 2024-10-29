// Fill out your copyright notice in the Description page of Project Settings.


#include "JBS/J_CaptureMissionMapActor.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/SceneComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AJ_CaptureMissionMapActor::AJ_CaptureMissionMapActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	sceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("sceneRoot"));
	SetRootComponent(sceneRoot);

	missionMapCapture2D = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("missionMapCapture2D"));
	missionMapCapture2D->SetupAttachment(sceneRoot);
}

// Called when the game starts or when spawned
void AJ_CaptureMissionMapActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AJ_CaptureMissionMapActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AJ_CaptureMissionMapActor::SetFov(float value)
{
	fov = value;
	OnRep_Fov();
}

void AJ_CaptureMissionMapActor::OnRep_Fov()
{
	missionMapCapture2D->FOVAngle = FOV;
}

void AJ_CaptureMissionMapActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AJ_CaptureMissionMapActor, fov);
}