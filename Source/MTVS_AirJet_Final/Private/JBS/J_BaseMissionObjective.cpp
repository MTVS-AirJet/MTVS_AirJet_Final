// Fill out your copyright notice in the Description page of Project Settings.


#include "JBS/J_BaseMissionObjective.h"
#include "Components/ArrowComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Engine/Engine.h"
#include "Engine/StaticMesh.h"
#include "JBS/J_Utility.h"
#include "Materials/Material.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
AJ_BaseMissionObjective::AJ_BaseMissionObjective()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	rootComp = CreateDefaultSubobject<USceneComponent>(TEXT("rootComp"));
	SetRootComponent(rootComp);

	sphereComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("sphereComp"));

	// 메시 및 머티리얼 로드
	ConstructorHelpers::FObjectFinder<UStaticMesh> tempSphere(
		TEXT("/Script/Engine.StaticMesh'/Engine/EngineMeshes/Sphere.Sphere'"));
	// 로드 성공시
	if(tempSphere.Succeeded())
	{
		sphereComp->SetStaticMesh(tempSphere.Object);

		// 머티리얼 가져오기
		ConstructorHelpers::FObjectFinder<UMaterialInterface> tempBasicMat(
			TEXT("/Script/Engine.Material'/Engine/MapTemplates/Materials/BasicAsset01.BasicAsset01'"));
		// 로드 성공시
		if(tempBasicMat.Succeeded())
		{
			sphereComp->SetMaterial(0, tempBasicMat.Object);
		}
	}
	// 게임에서 숨기기
	sphereComp->SetHiddenInGame(true);

	sphereComp->SetupAttachment(rootComp);

	forWComp = CreateDefaultSubobject<UArrowComponent>(TEXT("forWComp"));
	forWComp->SetupAttachment(sphereComp);
	forWComp->SetArrowSize(15.f);
	forWComp->SetArrowLength(75.f);



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
		// 목표 완료 딜리게이트 실행
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
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::White, FString::Printf(TEXT("%s 전술 명령 활성화"), *UEnum::GetValueAsString(orderType)));
}

void AJ_BaseMissionObjective::ObjectiveDeactive()
{
	
}

void AJ_BaseMissionObjective::SetSuccessPercent(float value)
{
	successPercent = value;
	// 수행도 갱신 딜리게이트 실행
	objSuccessUpdateDel.Broadcast();
	// 점수 갱신 딜리게이트 실행
	sendObjSuccessDel.Broadcast(this, SUCCESS_PERCENT);
}

void AJ_BaseMissionObjective::SRPC_StartNewObjUI_Implementation()
{

}

void AJ_BaseMissionObjective::SRPC_UpdateObjUI_Implementation()
{

}

void AJ_BaseMissionObjective::SRPC_EndObjUI_Implementation()
{

}
void AJ_BaseMissionObjective::SRPC_EndSubObjUI_Implementation() {}
