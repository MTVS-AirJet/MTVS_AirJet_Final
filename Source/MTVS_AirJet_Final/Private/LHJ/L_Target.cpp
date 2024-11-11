// Fill out your copyright notice in the Description page of Project Settings.


#include "LHJ/L_Target.h"

#include "MTVS_AirJet_Final.h"
#include "Components/WidgetComponent.h"
#include "LHJ/L_Viper.h"

// Sets default values
AL_Target::AL_Target()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TargetWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("TargetWidget"));
	TargetWidget->SetupAttachment(RootComponent);
	TargetWidget->SetDrawSize(FVector2D(120 , 120));
}

// Called when the game starts or when spawned
void AL_Target::BeginPlay()
{
	Super::BeginPlay();

	MyViper = Cast<AL_Viper>(GetOwner());
	//PrintNetLog();
}

// Called every frame
void AL_Target::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MyViper)
	{
		auto rt = (MyViper->GetActorLocation() - GetActorLocation()).Rotation();
		SetActorRotation(rt);
	}
}

void AL_Target::F_Destroy()
{
	this->Destroy();
}

void AL_Target::PrintNetLog()
{
	const FString conStr = GetNetConnection() ? TEXT("Valid Connection") : TEXT("Invalid Connection");
	const FString ownerName = GetOwner() ? GetOwner()->GetName() : TEXT("No Owner");

	FString logStr = FString::Printf(
		TEXT("Connection : %s\nPawn Name : %s\nOwner Name : %s\nLocal Role : %s\nRemote Role : %s") , *conStr ,
		*GetName() , *ownerName ,
		*LOCALROLE , *REMOTEROLE);
	FVector loc = GetActorLocation() + GetActorUpVector() * 30;
	DrawDebugString(GetWorld() , loc , logStr , nullptr , FColor::Yellow , 0 , true , 1.f);
}
