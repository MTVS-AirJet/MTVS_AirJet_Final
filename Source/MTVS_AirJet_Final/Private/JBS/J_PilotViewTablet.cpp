// Fill out your copyright notice in the Description page of Project Settings.


#include "JBS/J_PilotViewTablet.h"
#include "Components/SceneComponent.h"
#include "Components/WidgetComponent.h"
#include "JBS/J_PilotViewTabletUI.h"

// Sets default values
AJ_PilotViewTablet::AJ_PilotViewTablet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	tabletRoot = CreateDefaultSubobject<USceneComponent>(TEXT("tabletRoot"));
	SetRootComponent(tabletRoot);
	
	tabletUIComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("tabletUIComp"));
	tabletUIComp->SetupAttachment(tabletRoot);

}

// Called when the game starts or when spawned
void AJ_PilotViewTablet::BeginPlay()
{
	Super::BeginPlay();

	// ui 가져오기
	tabletUI = Cast<UJ_PilotViewTabletUI>(tabletUIComp->GetWidget());
	
}

// Called every frame
void AJ_PilotViewTablet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

