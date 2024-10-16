// Fill out your copyright notice in the Description page of Project Settings.


#include "JBS/J_PilotViewTablet.h"
#include "Components/SceneComponent.h"
#include "Components/WidgetComponent.h"
#include "JBS/J_PilotViewTabletUI.h"
#include "JBS/J_StreamingActor.h"
#include "Kismet/GameplayStatics.h"

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

	// FIXME 화면 공유 시작 이거 공유 버튼 눌러야하는거 내장 시켜야함.
	StartViewScreenShare();
}

// Called every frame
void AJ_PilotViewTablet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AJ_PilotViewTablet::StartViewScreenShare()
{
	auto* tempSA = Cast<AJ_StreamingActor>(
		UGameplayStatics::GetActorOfClass(GetWorld(), AJ_StreamingActor::StaticClass()));
	if(tempSA)
	{
		//해당 액터로 뭔가 하기
		tempSA->ChangeLookSharingScreen();
	}
}