// Fill out your copyright notice in the Description page of Project Settings.


#include "LHJ/L_Missile.h"

#include "MTVS_AirJet_Final.h"
#include "Math/MathFwd.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Components/BoxComponent.h"
#include "JBS/J_MissionActorInterface.h"
#include "LHJ/L_Viper.h"

// Sets default values
AL_Missile::AL_Missile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MissileBoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("MissileBoxComp"));
	SetRootComponent(MissileBoxComp);
	MissileBoxComp->OnComponentBeginOverlap.AddDynamic(this , &AL_Missile::OnMissileBeginOverlap);
	MissileBoxComp->SetBoxExtent(FVector(130 , 32 , 32));

	MissileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MissileMesh"));
	MissileMesh->SetupAttachment(RootComponent);

	EngineVFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("EngineVFX"));
	EngineVFX->SetupAttachment(RootComponent);
	EngineVFX->SetRelativeLocationAndRotation(FVector(-110 , 0 , 0) , FRotator(-90 , 180 , 180));

	SmokeVFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SmokeVFX"));
	SmokeVFX->SetupAttachment(RootComponent);
	SmokeVFX->SetRelativeLocation(FVector(-110 , 0 , 0));

	SetLifeSpan(10.f);

	bReplicates = true;
	SetReplicateMovement(true);
}

// Called when the game starts or when spawned
void AL_Missile::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
#pragma region TimeLine Settings
		FOnTimelineFloat ProgressUpdate;
		ProgressUpdate.BindUFunction(this , FName("MissileUpdate"));

		FOnTimelineEvent FinishedEvent;
		FinishedEvent.BindUFunction(this , FName("MissileFinished"));

		MissileTimeline.AddInterpFloat(MissileCurve , ProgressUpdate);
		MissileTimeline.SetTimelineFinishedFunc(FinishedEvent);
#pragma endregion

		if (auto viper = Cast<AL_Viper>(GetOwner()))
		{
			Target = viper->LockOnTarget;
			MoveLoc.Add(viper->GetActorLocation());
			MoveLoc.Add(viper->MissileMoveLoc->GetComponentLocation());
			MoveLoc.Add(viper->MissileMoveLoc->GetComponentLocation());
			MoveLoc.Add(Target->GetActorLocation());

			MissileTimeline.Play();
		}
	}
}

// Called every frame
void AL_Missile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (HasAuthority())
	{
		if (!bPursuit)
			MissileTimeline.TickTimeline(DeltaTime);
		else
		{
			if (!Target->IsPendingKillPending())
			{
				FVector P0 = GetActorLocation();
				FVector v = (Target->GetActorLocation() - P0);
				v.Normalize();
				FVector vt = v * MoveSpeed * DeltaTime;

				this->SetActorLocation(P0 + vt);
				this->SetActorRotation(v.Rotation());
			}
			else
			{
				this->Destroy();
			}
		}
	}
}

void AL_Missile::MissileUpdate(float Alpha)
{
	//LOG_S(Warning, TEXT("%f"), Alpha);
	FVector newLoc = BezierMissile(MoveLoc[0] , MoveLoc[1] , MoveLoc[2] , MoveLoc[3] , Alpha);
	this->SetActorLocation(newLoc);
}

void AL_Missile::MissileFinished()
{
	bPursuit = true;
}

FVector AL_Missile::BezierMissile(FVector P1 , FVector P2 , FVector P3 , FVector P4 , float Alpha)
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

void AL_Missile::OnMissileBeginOverlap(UPrimitiveComponent* OverlappedComponent , AActor* OtherActor ,
                                       UPrimitiveComponent* OtherComp , int32 OtherBodyIndex , bool bFromSweep ,
                                       const FHitResult& SweepResult)
{
	ServerRPCDamage(OtherActor);
}

void AL_Missile::ServerRPCDamage_Implementation(AActor* HitActor)
{
	LOG_S(Warning , TEXT("%s를 맞추었습니다.") , *HitActor->GetName());
	// 데미지 처리
	if (auto mai = Cast<IJ_MissionActorInterface>(HitActor))
	{
		mai->GetDamage();
		// UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), DistroyVFX, GetActorLocation(), );
		FVector VFXSpawnLoc = HitActor->GetActorLocation() + FVector::UpVector * 10000.f;
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), DistroyVFX, VFXSpawnLoc);
	}
	this->Destroy();
}

void AL_Missile::MulticastRPCDamage_Implementation(AActor* HitActor)
{
}
