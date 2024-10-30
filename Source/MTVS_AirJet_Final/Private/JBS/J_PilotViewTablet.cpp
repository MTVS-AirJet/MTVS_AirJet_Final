// Fill out your copyright notice in the Description page of Project Settings.


#include "JBS/J_PilotViewTablet.h"
#include "Components/SceneComponent.h"
#include "Components/WidgetComponent.h"
#include "Engine/Engine.h"
#include "JBS/J_PilotViewTabletUI.h"
#include "JBS/J_StreamingActor.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/OptionalFwd.h"
#include "Templates/NonNullPointer.h"

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
	// XXX 테스트용 플레이어 슬롯 ui 생성
	AddPlayerSlotUI(TArray<FString>());

	// 화면 시청 시작
	StartViewScreenShare();
}

// Called every frame
void AJ_PilotViewTablet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AJ_PilotViewTablet::StartViewScreenShare()
{
	// solved 스트리밍 액터 가져오기
	auto* tempSA = Cast<AJ_StreamingActor>(
		UGameplayStatics::GetActorOfClass(GetWorld(), AJ_StreamingActor::StaticClass()));
	if(tempSA)
	{
		//  XXX 유저 id 갱신?
		
		//해당 액터로 뭔가 하기
		// 시청 시작
		tempSA->ChangeLookSharingScreen();
		// GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Purple, TEXT("3. 태블릿 : 시청 시작"));
	}
}

void AJ_PilotViewTablet::AddPlayerSlotUI(TArray<FString> playerIdAry)
{
	tabletUI->AddPlayerSlot(playerIdAry);
}