// Fill out your copyright notice in the Description page of Project Settings.


#include "JBS/J_TacticalViewMonitor.h"
#include "Components/SceneComponent.h"
#include "Components/WidgetComponent.h"
#include "JBS/J_CaptureMissionMapActor.h"
#include "JBS/J_TacticalViewUI.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AJ_TacticalViewMonitor::AJ_TacticalViewMonitor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	rootComp = CreateDefaultSubobject<USceneComponent>(TEXT("rootComp"));
	SetRootComponent(rootComp);
	
	monitorRootComp = CreateDefaultSubobject<USceneComponent>(TEXT("monitorRootComp"));
	monitorRootComp->SetupAttachment(rootComp);

	monitorUIComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("monitorUIComp"));
	monitorUIComp->SetupAttachment(monitorRootComp);

}

// Called when the game starts or when spawned
void AJ_TacticalViewMonitor::BeginPlay()
{
	Super::BeginPlay();

	// ui 설정
	tacViewUI = Cast<UJ_TacticalViewUI>(monitorUIComp->GetWidget());

	// 캡처 카메라 가져오기
	auto* captureActor = Cast<AJ_CaptureMissionMapActor>(
		UGameplayStatics::GetActorOfClass(GetWorld(), AJ_CaptureMissionMapActor::StaticClass()));
	if(captureActor)
	{
		//해당 액터로 뭔가 하기
		mapCaptureActor = captureActor;
	}
	
}

// Called every frame
void AJ_TacticalViewMonitor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AJ_TacticalViewMonitor::SetMapFov(float value)
{
	// @@ 지도 확대 축소 로직 커서 위치에 맞게 옮겨가면 좋을듯

	// 일단 임시로 value / 100 을 alpha 값에 넣는 식으로
	fovPercent += value / 100.f;
	fovPercent = FMath::Clamp(fovPercent, 0.f, 1.f);
	
	// 결과 적용
	float result = FMath::Lerp(minFov, maxFov, fovPercent);
	mapCaptureActor->SetFov(result);
}