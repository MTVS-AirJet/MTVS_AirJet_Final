// Fill out your copyright notice in the Description page of Project Settings.


#include "JBS/J_BaseMissionObjective.h"
#include "Components/ArrowComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Containers/UnrealString.h"
#include "Engine/Engine.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/Pawn.h"
#include "JBS/J_ObjectiveIconUI.h"
#include "JBS/J_Utility.h"
#include "Materials/Material.h"
#include "JBS/J_CustomWidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "Templates/Casts.h"
#include "UObject/ConstructorHelpers.h"
#include "UObject/UObjectGlobals.h"

// Sets default values
AJ_BaseMissionObjective::AJ_BaseMissionObjective()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	bAlwaysRelevant = true;

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



	// iconWorldUIComp = CreateDefaultSubobject<UJ_CustomWidgetComponent>(TEXT("iconWorldUIComp"));

	iconWorldUIComp = CreateDefaultSubobject<UJ_CustomWidgetComponent>(TEXT("iconWorldUIComp"));
	iconWorldUIComp->SetupAttachment(RootComponent);

	// 위젯 블루프린트를 찾습니다.
	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetClassFinder(TEXT("/Game/Blueprints/UI/JBS/WBP_Objective3DIconUI"));
	// 위젯 클래스를 설정합니다.
	if (WidgetClassFinder.Succeeded())
		iconWorldUIComp->SetWidgetClass(WidgetClassFinder.Class);

	
}

// Called when the game starts or when spawned
void AJ_BaseMissionObjective::BeginPlay()
{
	Super::BeginPlay();
	
	// 아이콘 ui 설정
	auto* tempUI = CastChecked<UJ_ObjectiveIconUI>(iconWorldUIComp->GetWidget());
	if(tempUI)
		iconWorldUI = tempUI;

	objectiveActiveDel.AddUObject(this, &AJ_BaseMissionObjective::ObjectiveActive);
	objectiveDeactiveDel.AddUObject(this, &AJ_BaseMissionObjective::ObjectiveDeactive);


	// 로컬 pc 가져와서 pawn 넣기
	auto* pc =GetWorld()->GetFirstPlayerController();
	check(pc);
	if(pc->IsLocalPlayerController() || !pc->IsLocalPlayerController() && HasAuthority())
	{
		APawn* localPawn = pc->GetPawn();
		iconWorldUIComp->SetTargetActor(Cast<AActor>(localPawn));
	}
}

// Called every frame
void AJ_BaseMissionObjective::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 로컬 폰 과의 거리 설정
	auto* localPC = GetWorld()->GetFirstPlayerController();
	if(localPC->IsLocalPlayerController() || !localPC->IsLocalPlayerController() && HasAuthority())
	{
		auto* localPawn = localPC->GetPawn();
		float dis = FVector::Dist(this->GetActorLocation(), localPawn->GetActorLocation());
		iconWorldUI->SetObjDisText(dis);
	}
	
	// init 되기 전까지 무시
	if(orderType == ETacticalOrder::NONE) return;

	
	if(!HasAuthority()) return;
	
}

void AJ_BaseMissionObjective::ObjectiveSuccess()
{
	if(!HasAuthority()) return;
	// 미션 성공 딜리게이트 실행
	if(objectiveSuccessDel.IsBound())
	{
		objectiveSuccessDel.Broadcast();
	}
}

void AJ_BaseMissionObjective::ObjectiveFail()
{
	if(!HasAuthority()) return;
	// 미션 실패 딜리게이트 실행
	if(objectiveFailDel.IsBound())
	{
		objectiveFailDel.Broadcast();
	}
}

void AJ_BaseMissionObjective::ObjectiveEnd(bool isSuccess)
{
	if(!HasAuthority()) return;
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

	iconWorldUIComp->SetActive(isObjectiveActive);
	iconWorldUIComp->SetHiddenInGame(!isObjectiveActive);

	if(!HasAuthority()) return;
	// 활/비 딜리게이트 실행
	if(isObjectiveActive)
	{
		objectiveActiveDel.Broadcast();
	}
	else
	{
		objectiveDeactiveDel.Broadcast();
	}

	
	//@@
	// iconWorldUIComp->SetHiddenInGame(false);

}

void AJ_BaseMissionObjective::InitObjective(ETacticalOrder type, bool initActive)
{
	orderType = type;
	IS_OBJECTIVE_ACTIVE = initActive;
}

void AJ_BaseMissionObjective::ObjectiveActive()
{
	if(!HasAuthority()) return;
	// GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::White, FString::Printf(TEXT("%s 전술 명령 활성화"), *UEnum::GetValueAsString(orderType)));
	
}

void AJ_BaseMissionObjective::ObjectiveDeactive()
{
	if(!HasAuthority()) return;
	
}

void AJ_BaseMissionObjective::SetSuccessPercent(float value)
{
	if(!HasAuthority()) return;
	successPercent = value;
	// 수행도 갱신 딜리게이트 실행
	objSuccessUpdateDel.Broadcast();
	// 점수 갱신 딜리게이트 실행
	sendObjSuccessDel.Broadcast(this, SUCCESS_PERCENT);
}

void AJ_BaseMissionObjective::SRPC_StartNewObjUI_Implementation()
{
	if(!HasAuthority()) return;
}

void AJ_BaseMissionObjective::SRPC_UpdateObjUI_Implementation()
{
	if(!HasAuthority()) return;
}

void AJ_BaseMissionObjective::SRPC_EndObjUI_Implementation()
{
	if(!HasAuthority()) return;
}
void AJ_BaseMissionObjective::SRPC_EndSubObjUI_Implementation() {if(!HasAuthority()) return;}

void AJ_BaseMissionObjective::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AJ_BaseMissionObjective, isObjectiveActive);
}

void AJ_BaseMissionObjective::OnRep_ObjActive()
{
	iconWorldUIComp->SetActive(isObjectiveActive);
	iconWorldUIComp->SetHiddenInGame(!isObjectiveActive);
}
