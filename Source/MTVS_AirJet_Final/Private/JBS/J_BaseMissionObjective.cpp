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
#include "GameFramework/PlayerController.h"
#include "JBS/J_BaseMissionPawn.h"
#include "JBS/J_JsonUtility.h"
#include "JBS/J_MissionPlayerController.h"
#include "JBS/J_ObjectiveIconUI.h"
#include "JBS/J_ObjectiveUIComp.h"
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

	// 목표 ui 관련 딜리게이트 바인드
	// 활성화시 목표 UI 생성 바인드
    // objectiveActiveDel.AddUObject(this, &AJ_BaseMissionObjective::SRPC_StartNewObjUI);
    // FIXME 수행도 갱신시 목표 UI 값 갱신 바인드
    objSuccessUpdateDel.AddUObject(this, &AJ_BaseMissionObjective::SRPC_UpdateObjUI);
    // 목표 완료시 목표 UI 완료 바인드
    // objectiveEndDel.AddUObject(this, &AJ_BaseMissionObjective::SRPC_EndObjUI);
    // objectiveEndDel.AddUObject(this, &AJ_BaseMissionObjective::SRPC_EndSubObjUI);


	// 로컬 pc 가져와서 pawn 넣기
	auto* pc = GetWorld()->GetFirstPlayerController();
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
	auto* localPawn = UJ_Utility::GetBaseMissionPawn(GetWorld());
	// if(localPC->IsLocalPlayerController() || !localPC->IsLocalPlayerController() && HasAuthority())
	if(localPawn && localPawn->IsLocallyControlled())
	{
		// auto* localPawn = localPC->GetPawn();
		float dis = FVector::Dist(this->GetActorLocation(), localPawn->GetActorLocation());
		if(iconWorldUI)
			iconWorldUI->SetObjDisText(dis);
	}
	
	// init 되기 전까지 무시
	if(orderType == ETacticalOrder::NONE || !HasAuthority()) return;
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
	if(!HasAuthority() && !this->IS_OBJECTIVE_ACTIVE) return;
	// 미션 비활성화
	IS_OBJECTIVE_ACTIVE = false;
	// 완료 체크
	IS_OBJ_ENDED = true;

	// 목표 완료 딜리게이트 실행
	if(objectiveEndDel.IsBound())
		objectiveEndDel.Broadcast();

	// 성공 여부에 따라 함수 실행
	isSuccess ? ObjectiveSuccess() : ObjectiveFail();

	// GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("??? actor : %s"), *this->GetName()));
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
}

void AJ_BaseMissionObjective::InitObjective(ETacticalOrder type, bool initActive)
{
	orderType = type;
	IS_OBJECTIVE_ACTIVE = initActive;
}

void AJ_BaseMissionObjective::ObjectiveActive()
{
	if(!HasAuthority() || !IS_OBJECTIVE_ACTIVE) return;
	// GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::White, FString::Printf(TEXT("%s 전술 명령 활성화"), *UEnum::GetValueAsString(orderType)));

	// 목표 UI 신규 갱신 | movepoint에서 안써서 각자 하기로
	// SRPC_StartNewObjUI();

	// 1. 지휘관 보이스 라인 요청
	auto* gi = UJ_Utility::GetJGameInstance(GetWorld());
	gi->commanderVoiceResUseDel.BindUObject(this, &AJ_BaseMissionObjective::PlayCommanderVoiceToAll);
	
	FCommanderVoiceReq req(this->orderType);

	UJ_JsonUtility::RequestExecute(GetWorld(), EJsonType::COMMANDER_VOICE, req, gi);
}

void AJ_BaseMissionObjective::PlayCommanderVoiceToAll(const FCommanderVoiceRes &resData)
{
	// 2. 요청 한 보이스 라인 crpc로 재생
	// 모든 pc에게 crpc로 사운드 재생
	auto allPC = UJ_Utility::GetAllMissionPC(GetWorld());
	for(auto* pc : allPC)
	{
		pc->CRPC_PlayCommanderVoice(resData.voice);
	}
}

void AJ_BaseMissionObjective::ObjectiveDeactive()
{
	if(!HasAuthority() || IS_OBJECTIVE_ACTIVE) return;
	
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
	if(!HasAuthority() || !IS_OBJECTIVE_ACTIVE) return;

	// 모든 pc 가져오기
	auto allPC = UJ_Utility::GetAllMissionPC(GetWorld());

    // pc에게 새 전술명령 UI 시작 crpc
    for(auto* pc : allPC)
    {
		// 보낼 목표 데이터 구성
		FTacticalOrderData orderData = SetObjUIData(pc);
		// ui 생성 시작
        pc->objUIComp->CRPC_StartObjUI(orderData);
    }
}

FTacticalOrderData AJ_BaseMissionObjective::SetObjUIData(AJ_MissionPlayerController* pc)
{
	return FTacticalOrderData();
}

void AJ_BaseMissionObjective::SRPC_UpdateObjUI_Implementation()
{
	if(!HasAuthority() || !IS_OBJECTIVE_ACTIVE) return;

	// 보낼 데이터
    // 모든 pc 가져오기
    auto allPC = UJ_Utility::GetAllMissionPC(GetWorld());

    // pc에게 새 전술명령 UI 시작 srpc
    for(auto* pc : allPC)
    {
		auto orderData = SetObjUIData(pc);
		
		
		// 과도한 crpc 방지 처리
		if(!prevObjUIDataMap.Contains(pc) || orderData != prevObjUIDataMap[pc])
		{
			// 데이터 보내기
			pc->objUIComp->CRPC_UpdateObjUI(orderData);
			// objui데이터 맵에 저장
			prevObjUIDataMap.Add(pc, orderData);
		}
    }
}

void AJ_BaseMissionObjective::SRPC_EndObjUI_Implementation()
{
	if(!HasAuthority()) return;

	// 모든 pc 가져오기
    auto allPC = UJ_Utility::GetAllMissionPC(GetWorld());

    // pc에게 새 전술명령 UI 시작 srpc
    for(auto* pc : allPC)
    {
        pc->objUIComp->CRPC_EndObjUI();
    }
}
void AJ_BaseMissionObjective::SRPC_EndSubObjUI_Implementation(AJ_MissionPlayerController* pc, int idx, bool isSuccess)
{
	if(!HasAuthority()) return;
	// GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::White, FString::Printf(TEXT("%s"), *pc->GetName()));
	pc->objUIComp->CRPC_EndSubObjUI(idx, isSuccess);

	// // 모든 pc 가져오기
    // auto allPC = UJ_Utility::GetAllMissionPC(GetWorld());

    // // pc에게 새 전술명령 UI 시작 srpc
    // for(auto* pc : allPC)
    // {
    // }
}

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


