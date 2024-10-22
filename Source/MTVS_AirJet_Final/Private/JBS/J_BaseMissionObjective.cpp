// Fill out your copyright notice in the Description page of Project Settings.


#include "JBS/J_BaseMissionObjective.h"
#include "Components/SceneComponent.h"
#include "Components/WidgetComponent.h"
#include "JBS/J_Utility.h"

// Sets default values
AJ_BaseMissionObjective::AJ_BaseMissionObjective()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	rootComp = CreateDefaultSubobject<USceneComponent>(TEXT("rootComp"));
	SetRootComponent(rootComp);

	iconWorldUIComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("iconWorldUIComp"));
	iconWorldUIComp->SetupAttachment(RootComponent);
	
	

}

// Called when the game starts or when spawned
void AJ_BaseMissionObjective::BeginPlay()
{
	Super::BeginPlay();
	
	// @@ 나중에 만들면 캐스팅해서 가져오기
	// 아이콘 ui 설정
	auto* tempUI = iconWorldUIComp->GetWidget();
	if(tempUI)
	{
		iconWorldUI = tempUI;
	}

}

// Called every frame
void AJ_BaseMissionObjective::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// init 되기 전까지 무시
	if(orderType == ETacticalOrder::NONE) return;


}

void AJ_BaseMissionObjective::ObjectiveSuccess()
{
	// 미션 성공 딜리게이트 실행
	if(objectiveSuccessDel.IsBound())
	{
		objectiveSuccessDel.Broadcast();
	}
}

void AJ_BaseMissionObjective::ObjectiveFail()
{
	// 미션 실패 딜리게이트 실행
	if(objectiveFailDel.IsBound())
	{
		objectiveFailDel.Broadcast();
	}
}

void AJ_BaseMissionObjective::ObjectiveEnd(bool isSuccess)
{
	// 미션 비활성화
	IS_OBJECTIVE_ACTIVE = false;

	if(objectiveEndDel.IsBound())
	{
		objectiveEndDel.Broadcast();
	}

	// 성공 여부에 따라 함수 실행
	isSuccess ? ObjectiveSuccess() : ObjectiveFail();
}

void AJ_BaseMissionObjective::SetObjectiveActive(bool value)
{
	isObjectiveActive = value;

	// 활/비 딜리게이트 실행
	if(isObjectiveActive)
	{
		objectiveActiveDel.Broadcast();
	}
	else
	{
		objectiveDeactiveDel.Broadcast();
	}

	iconWorldUIComp->SetActive(isObjectiveActive);

}

void AJ_BaseMissionObjective::InitObjective(ETacticalOrder type, bool initActive)
{
	orderType = type;
	IS_OBJECTIVE_ACTIVE = initActive;
}

void AJ_BaseMissionObjective::ObjectiveActive()
{
	
}

void AJ_BaseMissionObjective::ObjectiveDeactive()
{
	
}


