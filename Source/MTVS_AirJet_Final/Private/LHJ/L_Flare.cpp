// Fill out your copyright notice in the Description page of Project Settings.


#include "LHJ/L_Flare.h"

#include "NiagaraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SplineComponent.h"
#include "LHJ/L_Viper.h"

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

	if (HasAuthority())
	{
#pragma region TimeLine Settings
		FOnTimelineFloat ProgressUpdate;
		ProgressUpdate.BindUFunction(this , FName("FlareUpdate"));

		FOnTimelineEvent FinishedEvent;
		FinishedEvent.BindUFunction(this , FName("FlareFinished"));

		FlareTimeline.AddInterpFloat(FlareCurve , ProgressUpdate);
		FlareTimeline.SetTimelineFinishedFunc(FinishedEvent);
#pragma endregion

		if (auto viper = Cast<AL_Viper>(GetOwner()))
		{
			MoveLoc.Add(viper->GetActorLocation());
			MoveLoc.Add(viper->FlareMoveLoc1->GetComponentLocation());
			MoveLoc.Add(viper->FlareMoveLoc2->GetComponentLocation());
			MoveLoc.Add(viper->FlareMoveLoc3->GetComponentLocation());

			FlareTimeline.Play();
		}
	}

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

	if (HasAuthority())
	{
		if (!bPursuit)
			FlareTimeline.TickTimeline(DeltaTime);
		else
		{
			FVector FlareLocation = GetActorLocation();
			FVector FlareUpVector = GetActorUpVector() * -1;
			FVector vt = FlareUpVector * FlareSpeed * DeltaTime;
			SetActorLocation(FlareLocation + vt);
		}
	}	
}

void AL_Flare::OnFlareBeginOverlap(UPrimitiveComponent* OverlappedComponent , AActor* OtherActor ,
                                   UPrimitiveComponent* OtherComp , int32 OtherBodyIndex , bool bFromSweep ,
                                   const FHitResult& SweepResult)
{
	ServerRPCFlare();
}

void AL_Flare::FlareUpdate(float Alpha)
{
	FVector newLoc = BezierFlare(MoveLoc[0] , MoveLoc[1] , MoveLoc[2] , MoveLoc[3] , Alpha);
	this->SetActorLocation(newLoc);
}

void AL_Flare::FlareFinished()
{
	bPursuit = true;
}

FVector AL_Flare::BezierFlare(FVector P1, FVector P2, FVector P3, FVector P4, float Alpha)
{
	//UKismetMathLibrary::VLerp
	//A + V * (B - A)
	FVector L1 = P1 + Alpha * (P2 - P1); //P1, P2
	FVector L2 = P2 + Alpha * (P3 - P2); //P2, P3
	FVector L3 = P3 + Alpha * (P4 - P3); //P3, P4

	FVector L4 = L1 + Alpha * (L2 - L1); //L1, L2
	FVector L5 = L2 + Alpha * (L3 - L2); //L2, L3

	FVector L6 = L4 + Alpha * (L5 - L4); //L4, L5

	return L6;
}

void AL_Flare::ServerRPCFlare_Implementation()
{
}
