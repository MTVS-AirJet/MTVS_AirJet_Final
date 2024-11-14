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
#include "JBS/J_Utility.h"
#include "JBS/J_BaseMissionPawn.h"
#include "JBS/J_MissionPlayerController.h"
#include "JBS/J_ObjectiveIconUI.h"
#include "JBS/J_ObjectiveUIComp.h"
#include "JBS/J_CustomWidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "Templates/Casts.h"
#include "UObject/ConstructorHelpers.h"
#include "UObject/UObjectGlobals.h"

// Sets default values
// 생성자
AJ_BaseMissionObjective::AJ_BaseMissionObjective()
{
	PrimaryActorTick.bCanEverTick = true;
	// 리플리케이트 | 클라에도 보여야함
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

	// 3d icon
	iconWorldUIComp = CreateDefaultSubobject<UJ_CustomWidgetComponent>(TEXT("iconWorldUIComp"));
	iconWorldUIComp->SetupAttachment(RootComponent);
	iconWorldUIComp->SetIsReplicated(true);
	iconWorldUIComp->SetCollisionProfileName(FName(TEXT("NoCollision")));

	// 위젯 블루프린트를 찾습니다.
	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetClassFinder(TEXT("/Game/Blueprints/UI/JBS/WBP_Objective3DIconUI"));
	// 위젯 클래스를 설정합니다.
	if (WidgetClassFinder.Succeeded())
		iconWorldUIComp->SetWidgetClass(WidgetClassFinder.Class);
}

// Called when the game starts or when spawned
#pragma region 시작 설정 단
/* icon UI 캐시 -> (서버단) 딜리게이트 바인드 -> init 목표 초기 설정 -> set active(false)*/
void AJ_BaseMissionObjective::BeginPlay()
{
	Super::BeginPlay();
	
	// 아이콘 ui 캐시
	auto* tempUI = CastChecked<UJ_ObjectiveIconUI>(iconWorldUIComp->GetWidget());
	if(tempUI)
		iconWorldUI = tempUI;

	// 로컬 pc의 전투기 바라보도록 설정
	SetTargetIconUI();

#pragma region begin 딜리게이트 바인드 단
	if(HasAuthority())
	{
		// 활/비활성화 함수 바인드
		objectiveActiveDel.AddUObject(this, &AJ_BaseMissionObjective::ObjectiveActive);
		objectiveDeactiveDel.AddUObject(this, &AJ_BaseMissionObjective::ObjectiveDeactive);

		// 수행도 갱신시 목표 UI 값 갱신 바인드
		objSuccessUpdateDel.AddUObject(this, &AJ_BaseMissionObjective::UpdateObjUI);
		// 목표 완료시 목표 UI 완료 바인드
		objectiveEndDel.AddUObject(this, &AJ_BaseMissionObjective::EndObjUI);
	}
#pragma endregion
	
}

void AJ_BaseMissionObjective::SetTargetIconUI()
{
	auto* pc = GetWorld()->GetFirstPlayerController();
	if(!pc) return;

	if(pc->IsLocalPlayerController() 
	|| !pc->IsLocalPlayerController() && HasAuthority())
	{
		APawn* localPawn = pc->GetPawn();
		if(!localPawn) return;
		
		iconWorldUIComp->SetTargetActor(Cast<AActor>(localPawn));
	}
}

// 목표 타입, 초기 활성 여부 (false 만 사용) 설정
void AJ_BaseMissionObjective::InitObjective(ETacticalOrder type, bool initActive)
{
	orderType = type;
	IS_OBJECTIVE_ACTIVE = initActive;
}

// 활성 유무 설정
void AJ_BaseMissionObjective::SetObjectiveActive(bool value)
{
	if(!HasAuthority() || IS_OBJ_ENDED) return;
	// 값 설정
	isObjectiveActive = value;
	// iconui 활/비
	iconWorldUIComp->MRPC_SetVisible(value);
	// 활/비 딜리게이트 실행
	if(isObjectiveActive)
		objectiveActiveDel.Broadcast();
	else
		objectiveDeactiveDel.Broadcast();
}

void AJ_BaseMissionObjective::ObjectiveActive()
{
	if(!HasAuthority() || IS_OBJ_ENDED) return;
}

void AJ_BaseMissionObjective::ObjectiveDeactive()
{
	if(!HasAuthority() || IS_OBJ_ENDED) return;
}

#pragma endregion

#pragma region tick 반복 단
/* 로컬 폰과의 거리 icon UI 설정 -> (서버단) tick 시작
   수행도 갱신시 갱신된 수행도를 목표 매니저에게 보냄*/
void AJ_BaseMissionObjective::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// icon ui 로컬 폰 과의 거리 설정
	UpdateObjDisIconUI();
	
	// init 되기 전까지 무시
	if(!HasAuthority() || orderType == ETacticalOrder::NONE) return;
}

void AJ_BaseMissionObjective::UpdateObjDisIconUI()
{
	if(!iconWorldUI) return;

	auto* localPawn = UJ_Utility::GetBaseMissionPawn(GetWorld());
	if(localPawn && localPawn->IsLocallyControlled())
	{
		// 거리 | 나 - 로컬
		float dis = FVector::Dist(this->GetActorLocation(), localPawn->GetActorLocation());

		iconWorldUI->SetObjDisText(dis);
	}
	else if(!localPawn)
		UE_LOG(LogTemp, Warning, TEXT("목표 액터 : 로컬 폰 없음"));
}

void AJ_BaseMissionObjective::SetSuccessPercent(float value)
{
	if(!HasAuthority()) return;

	successPercent = value;
	// 수행도 갱신 딜리게이트 실행
	objSuccessUpdateDel.Broadcast();
	// 점수 갱신 딜리게이트 실행 -> 목표 매니저에게 보냄
	sendObjSuccessDel.Broadcast(this, SUCCESS_PERCENT);
}

#pragma endregion

#pragma region 목표 종료 단
/* (서버단 && 활성화중) 목표 종료 -> 딜리게이트 -> 성공/실패 처리 */
void AJ_BaseMissionObjective::ObjectiveEnd(bool isSuccess)
{
	if(!HasAuthority() || !this->IS_OBJECTIVE_ACTIVE || IS_OBJ_ENDED) return;

	// 미션 비활성화
	IS_OBJECTIVE_ACTIVE = false;
	// 완료 체크
	IS_OBJ_ENDED = true;

	// 목표 완료 딜리게이트 실행
	if(objectiveEndDel.IsBound())
		objectiveEndDel.Broadcast();

	// 성공 여부에 따라 함수 실행
	isSuccess ? ObjectiveSuccess() : ObjectiveFail();

	// FString debugStr = FString::Printf(TEXT("완료된 목표 : %s"), *this->GetName());
	// UJ_Utility::PrintFullLog(debugStr, 10, FColor::White);
}

void AJ_BaseMissionObjective::ObjectiveSuccess()
{
	if(!HasAuthority()) return;

	// 미션 성공 딜리게이트 실행
	if(objectiveSuccessDel.IsBound())
		objectiveSuccessDel.Broadcast();
}

void AJ_BaseMissionObjective::ObjectiveFail()
{
	if(!HasAuthority()) return;

	// 미션 실패 딜리게이트 실행
	if(objectiveFailDel.IsBound())
		objectiveFailDel.Broadcast();
}

#pragma endregion

#pragma region 목표 UI 적용 단
/* 특정 타이밍에 목표 UI 시작 | StartNewObjUI
-> 각 pc마다 목표 ui 데이터 설정 | SendObjUIData
-> 특정 갱신 타이밍 마다 업데이트 UI | UpdateObjUI -> 이전에 보냈는지 체크후 업데이트 | CheckSendSameData
-> 목표 종료 나 서브 목표 종료시 ui 애니메이션 실행 요청 | EndObjUI , EndSubObjUI
-> 목표 종료시 비작동*/
void AJ_BaseMissionObjective::StartNewObjUI() 
{
	if(!HasAuthority() || IS_OBJ_ENDED) return;

	// 모든 pc 가져오기
	const auto& allPC = UJ_Utility::GetAllMissionPC(GetWorld());

    // pc에게 새 전술명령 UI 시작 crpc
    for(auto* pc : allPC)
    {
		// 시작 ui 설정
		SendObjUIData(pc, true);
    }
}

void AJ_BaseMissionObjective::SendObjUIData(class AJ_MissionPlayerController *pc, bool isInit)
{
	if(!HasAuthority()) return;

	// 재정의 필수
	// 데이터 구성
	// pc->objuicomp->crpc ui 적용
	// start 유무에 따라 다른 함수 적용
	// 
	// @@ 템플릿 쓸 수 있을지 고민
	/*// 보낼 목표 데이터 구성
		FTacticalOrderData orderData = SetObjUIData(pc);
		// ui 생성 시작
        pc->objUIComp->CRPC_StartObjUI(orderData);
		*/
}

void AJ_BaseMissionObjective::UpdateObjUI()
{
	if(!HasAuthority()) return;

    // 모든 pc 가져오기
    const auto& allPC = UJ_Utility::GetAllMissionPC(GetWorld());
    // pc에게 새 전술명령 UI 시작 srpc
    for(auto* pc : allPC)
    {
		// 과도한 crpc 방지 처리
		// 이전에 보낸 ui 데이터와 동일한지 검증
		// setobjuidata
		// 다르면 업데이트
		// @@ 템플릿 쓸 수 있을지 고민
		FTacticalOrderData temp;
		bool canUpdate = CheckSendSameData(pc, temp);
		if(canUpdate)
		{
			// 데이터 보내기
			// pc->objUIComp->CRPC_UpdateObjUI(orderData);
		}
    }
}

bool AJ_BaseMissionObjective::CheckSendSameData(class AJ_MissionPlayerController* pc, const FTacticalOrderData& uiData)
{	
	// 동일 ui 체크
	if(!prevObjUIDataMap.Contains(pc)) return false;
	if(uiData != prevObjUIDataMap[pc]) return false;

	// objui데이터 맵에 저장
	prevObjUIDataMap.Add(pc, uiData);

	return true;
}

void AJ_BaseMissionObjective::EndSubObjUI(AJ_MissionPlayerController* pc, int idx, bool isSuccess)
{
	if(!HasAuthority()) return;

	pc->objUIComp->CRPC_EndSubObjUI(idx, isSuccess);
}

void AJ_BaseMissionObjective::EndObjUI()
{
	if(!HasAuthority()) return;

	// 모든 pc 가져오기
    const auto& allPC = UJ_Utility::GetAllMissionPC(GetWorld());

    // pc에게 새 전술명령 UI 시작 srpc
    for(auto* pc : allPC)
    {
        pc->objUIComp->CRPC_EndObjUI();
    }
}
#pragma endregion

void AJ_BaseMissionObjective::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// DOREPLIFETIME(AJ_BaseMissionObjective, isObjectiveActive);
}

// ai 지휘관 보이스 재생 요청 | 배열로 해서 세밀하게 조절 가능
void AJ_BaseMissionObjective::ReqPlayCommVoice(int idx, const TArray<class AJ_MissionPlayerController*>& pcs)
{
	for(auto* pc : pcs)
	{
		pc->CRPC_PlayCommanderVoice3(idx);
	}
}
