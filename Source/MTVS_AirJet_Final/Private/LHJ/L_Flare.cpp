// Fill out your copyright notice in the Description page of Project Settings.


#include "LHJ/L_Flare.h"

#include "NiagaraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SplineComponent.h"

// Sets default values
AL_Flare::AL_Flare()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FlareBoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("FlareBoxComp"));
	SetRootComponent(FlareBoxComp);
	FlareBoxComp->SetSimulatePhysics(true);
	FlareBoxComp->OnComponentBeginOverlap.AddDynamic(this , &AL_Flare::OnFlareBeginOverlap);

	FlareMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FlareMesh"));
	FlareMesh->SetupAttachment(RootComponent);

	FlareEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("FlareEffect"));
	FlareEffect->SetupAttachment(RootComponent);

	FlareSpline = CreateDefaultSubobject<USplineComponent>(TEXT("FlareSpline"));
	FlareSpline->SetupAttachment(FlareEffect);


	SetLifeSpan(10.f);

	bReplicates = true;
	SetReplicateMovement(true);
}

// Called when the game starts or when spawned
void AL_Flare::BeginPlay()
{
	Super::BeginPlay();

	FlareEffect->SetVariableObject(FName("SplineObject") , FlareSpline);
	FlareEffect->SetFloatParameter(FName("MaxLifetime") , MaxLifeTime);
	FlareEffect->SetFloatParameter(FName("MinLifetime") , MinLifeTime);
	FlareEffect->SetFloatParameter(FName("MaxScaleSprite") , MaxScaleSprite);
	FlareEffect->SetFloatParameter(FName("MinScaleSprite") , MinScaleSprite);
	FlareEffect->SetFloatParameter(FName("SpawnRadius") , SpawnRadius);
	FlareEffect->SetFloatParameter(FName("SpawnRate") , SpawnRate);
	FlareEffect->SetColorParameter(FName("Color") , LColor);
}

// Called every frame
void AL_Flare::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector FlareLocation = GetActorLocation();
	FVector FlareUpVector = GetActorUpVector() * -1;
	FVector vt = FlareUpVector * FlareSpeed * DeltaTime;
	SetActorLocation(FlareLocation + vt);
}

void AL_Flare::OnFlareBeginOverlap(UPrimitiveComponent* OverlappedComponent , AActor* OtherActor ,
                                   UPrimitiveComponent* OtherComp , int32 OtherBodyIndex , bool bFromSweep ,
                                   const FHitResult& SweepResult)
{
	ServerRPCFlare();
}

void AL_Flare::ServerRPCFlare_Implementation()
{
}
