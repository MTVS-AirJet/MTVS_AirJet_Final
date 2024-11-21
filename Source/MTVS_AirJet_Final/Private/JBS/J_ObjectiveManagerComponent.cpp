// Fill out your copyright notice in the Description page of Project Settings.


#include "JBS/J_ObjectiveManagerComponent.h"
#include "Engine/Engine.h"
#include "Engine/EngineTypes.h"
#include "Engine/World.h"
#include "JBS/J_BaseMissionObjective.h"
#include "JBS/J_MissionGamemode.h"
#include "JBS/J_Utility.h"
#include "Kismet/GameplayStatics.h"
#include "JBS/J_MissionPlayerController.h"
#include "JBS/J_ObjectiveUIComp.h"
#include "Math/MathFwd.h"
#include "TimerManager.h"

// Sets default values for this component's properties
UJ_ObjectiveManagerComponent::UJ_ObjectiveManagerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UJ_ObjectiveManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	// 주인 게임모드 설정
	ownerGM = this->GetOwner<AJ_MissionGamemode>();

	// 이륙 시 기본 목표 스킵 바인드
	ownerGM->startTODel.AddUObject(this, &UJ_ObjectiveManagerComponent::SkipDefaultObj);
}


// Called every frame
void UJ_ObjectiveManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	if(enablePrintCurActiveMissionActor)
	{
		if(!CUR_ACTIVE_MISSION) return;
		
		GEngine->AddOnScreenDebugMessage(-1, -1.f, FColor::Purple, FString::Printf(TEXT("현재 활성화된 목표 : %s"), *CUR_ACTIVE_MISSION->GetName()));
	}
}

void UJ_ObjectiveManagerComponent::InitDefaultObj()
{
	if(!GetOwner()->HasAuthority()) return;
	// solved 전술명령 목표와는 별개로 동작하면서 비슷한 로직으로 구성
	
	for(auto& dmData : defaultObjDataAry)
	{
		// 프리팹 가져오기
		auto* objActor = SpawnObjActor(dmData.objType);

		// 목표 액터 설정
		dmData.objectiveActor = objActor;

		// 목표 수행도 갱신함수 바인드
		objActor->sendObjSuccessDel.AddUObject(this, &UJ_ObjectiveManagerComponent::UpdateObjectiveSuccess);
		
		// 시동 절차 끝나고 이륙 절차 시작
		if(dmData.objType == ETacticalOrder::ENGINE_START)
		{
			objActor->objectiveEndDel.AddDynamic( this, &UJ_ObjectiveManagerComponent::StartTakeOffObj);
		}
	}
}

// 기본 목표 시작 | 시동
void UJ_ObjectiveManagerComponent::StartDefualtObj()
{
	// GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("응애에요"));

	auto* engineObj = defaultObjDataAry[0].objectiveActor;
	check(engineObj);

	// 활성화
	DelayedObjectiveActive(engineObj, 0.001f);
}

void UJ_ObjectiveManagerComponent::InitObjectiveList(TArray<struct FMissionObject> missions)
{
	// 목표 배열 초기화
	objectiveDataAry.Init(FObjectiveData(), missions.Num());

	// 데이터 가지고 미션 액터 하나씩 생성
	for(auto mData : missions)
	{
		// 스폰 지점 계산
		FTransform spawnTR = mData.GetTransform();
		spawnTR.SetRotation(FQuat(FRotator::ZeroRotator));

		// 목표 액터 스폰
		auto* objectiveActor = SpawnObjActor(mData.GetOrderType(), spawnTR);

		// 목표 완료시 다음 목표 활성화 바인드
		objectiveActor->objectiveEndDel.AddDynamic(this, &UJ_ObjectiveManagerComponent::ActiveNextObjective);
		// 목표 수행도 갱신함수 바인드
		objectiveActor->sendObjSuccessDel.AddUObject(this, &UJ_ObjectiveManagerComponent::UpdateObjectiveSuccess);
	
		// 목표 액터 배열에 추가
		// 순서대로 하기위해 pinNo 사용
		objectiveDataAry[mData.pinNo].objectiveActor = objectiveActor;
		objectiveDataAry[mData.pinNo].objType = objectiveActor->orderType;
		// objectiveAry.Add(objectiveActor);
	}

	// GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::White, FString::Printf(TEXT("목표 액터 추가 완료 개수 : %d"), objectiveDataAry.Num()));
}

// 목표 액터 스폰 
AJ_BaseMissionObjective*  UJ_ObjectiveManagerComponent::SpawnObjActor(ETacticalOrder type, const FTransform &spawnTR)
{
	// 목표 종류에 따라 프리팹 가져오기
	auto prefab = objectiveActorPrefabMap[type];

	// 항상 스폰 처리
	FActorSpawnParameters params;
	params.bNoFail = true;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// 목표 액터 스폰
	auto* objectiveActor = GetWorld()->SpawnActor<AJ_BaseMissionObjective>(prefab, spawnTR, params);

	// 목표 액터 설정
	objectiveActor->InitObjective(type, false);
	

	return objectiveActor;
}

void UJ_ObjectiveManagerComponent::ActiveObjectiveByIdx(int mIdx, bool isFirst)
{
	if(mIdx >= objectiveDataAry.Num())
	{
		// GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("objAry out of range"));

		return;
	}
	// 목표 액터 가져오기
	auto* obj = objectiveDataAry[mIdx].objectiveActor;
	if(!obj) return;
	
	// 딜레이 여부
	float delayTime = isFirst ? 0.001f : objSwitchInterval;

	// 활성화
	DelayedObjectiveActive(obj, delayTime);
}

void UJ_ObjectiveManagerComponent::ActiveNextObjective()
{
	// 인덱스 증가
	CUR_ACTIVE_MISSION_IDX++;
	// 여기서 목표 다했으면 결산으로 넘어가짐 setcuractive~~
	if(isMissionComplete) return;

	// 최초가 아니면 종료 애니메이션 대기 (delay 애니메이션에 맞게 수정 필요)
	ActiveObjectiveByIdx(CUR_ACTIVE_MISSION_IDX, CUR_ACTIVE_MISSION_IDX == 0);
}

void UJ_ObjectiveManagerComponent::SetCurActiveMissionIdx(int value)
{
	if(!GetOwner()->HasAuthority()) return;
	
	if(value < 0 || value > objectiveDataAry.Num())
	{
		// GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("idx value 문제"));
		return;
	}

    curActiveMissionIdx = value;
    if (CUR_ACTIVE_MISSION_IDX >= objectiveDataAry.Num())
    {
        // GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::White, TEXT("목표 종료됨"));

        // 미션 결산
		MissionComplete();

		return;
    }

    // 활성 미션 액터 설정 | delay active 쪽에서 하는중
    // CUR_ACTIVE_MISSION = objectiveDataAry[value].objectiveActor;
}

void UJ_ObjectiveManagerComponent::MissionComplete()
{
	// GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::White, TEXT("미션 종료 결산 화면으로"));

	isMissionComplete = true;

	// 여기부터 결산 단계
	// 시동,이륙, 목표 데이터를 다 가진 전체 데이터
	TArray<FObjectiveData> fullObjData;
	fullObjData.Append(defaultObjDataAry);
	fullObjData.Append(objectiveDataAry);
	
	// 미션 완료 ui 전환
	FTimerHandle timerHandle;
	GetWorld()->GetTimerManager()
		.SetTimer(timerHandle, [this, fullObjData]() mutable
	{
		//타이머에서 할 거
		auto allPC = UJ_Utility::GetAllMissionPC(GetWorld());

		for(AJ_MissionPlayerController* pc : allPC)
		{
			// 결산 UI 전환
			pc->objUIComp->CRPC_SwitchResultUI(fullObjData);
		}
	}, objSwitchInterval, false);
}

void UJ_ObjectiveManagerComponent::UpdateObjectiveSuccess(AJ_BaseMissionObjective* objActor, float successPercent)
{
	// 사용 목표 찾기
	FObjectiveData* data = defaultObjDataAry.FindByPredicate([objActor](const FObjectiveData& objData)
	{
		return objData.objectiveActor == objActor;
	});

	if(!data)
	{
		data = objectiveDataAry.FindByPredicate([objActor](const FObjectiveData& objData)
		{
			return objData.objectiveActor == objActor;
		});
	}
	// 내가 기어코 구조체 포인터를 쓰는구나

	// 갱신
	data->successPercent = successPercent;

	// GEngine->AddOnScreenDebugMessage(-1, -1.f, FColor::Green, FString::Printf(TEXT("도당체 : %.2f"), data.successPercent));
}

void UJ_ObjectiveManagerComponent::DelayedObjectiveActive(AJ_BaseMissionObjective *obj, float delayTime)
{
	check(obj);
	// GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("dowdiwad"));
	FTimerHandle timerHandle2;
	// check(UJ_Utility::GetBaseMissionPawn(GetWorld()));

	GetWorld()->GetTimerManager()
		.SetTimer(timerHandle2, [this, obj]() mutable
	{
		// check(UJ_Utility::GetBaseMissionPawn(GetWorld()));
		obj->IS_OBJECTIVE_ACTIVE = true;
		CUR_ACTIVE_MISSION = obj;		
		// GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString::Printf(TEXT("asdasd %s"), *obj->GetName()));
	}, delayTime, false);
}

void UJ_ObjectiveManagerComponent::SkipDefaultObj(bool isSuccess)
{
	for(const FObjectiveData& objData : defaultObjDataAry)
	{
		auto* objActor = objData.objectiveActor;
		if(!objActor) continue;
		if(!objActor->IS_OBJ_ENDED) continue;

		// 수행도는 0으로
		objActor->SUCCESS_PERCENT = 0.f;
		// 종료
		objActor->ObjectiveEnd(isSuccess);
	}
}
void UJ_ObjectiveManagerComponent::StartTakeOffObj()
{
	// 미션 시작 시 활성화 안함
	if(CUR_ACTIVE_MISSION_IDX >= 0) return;

	auto* takeOffObj = defaultObjDataAry[1].objectiveActor;
	if(ownerGM->isTPReady) return;
	if(!takeOffObj) return;
	if(takeOffObj->IS_OBJ_ENDED) return;

	// 활성화
	DelayedObjectiveActive(takeOffObj, objSwitchInterval);
}
