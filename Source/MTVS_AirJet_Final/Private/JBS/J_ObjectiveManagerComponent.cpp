// Fill out your copyright notice in the Description page of Project Settings.


#include "JBS/J_ObjectiveManagerComponent.h"
#include "Engine/World.h"
#include "JBS/J_BaseMissionObjective.h"
#include "JBS/J_MissionGamemode.h"
#include "JBS/J_Utility.h"

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
}


// Called every frame
void UJ_ObjectiveManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UJ_ObjectiveManagerComponent::InitObjectiveList(TArray<struct FMissionObject> missions)
{
	// 목표 배열 초기화
	objectiveDataAry.Init(FObjectiveData(), missions.Num());

	// 데이터 가지고 미션 액터 하나씩 생성
	for(auto mData : missions)
	{
		// 목표 종류에 따라 프리팹 가져오기
		// 목표 종류
		auto type = mData.GetOrderType();
		// 프리팹
		auto prefab = objectiveActorPrefabMap[type];
		// 스폰 지점 계산
		FTransform spawnTR = mData.GetTransform();

		// 목표 액터 스폰
		auto* objectiveActor = GetWorld()->SpawnActor<AJ_BaseMissionObjective>(prefab, spawnTR);

		// 목표 액터 설정
		objectiveActor->InitObjective(type, false);
		// 목표 완료시 다음 목표 활성화 바인드
		objectiveActor->objectiveEndDel.AddUObject(this, &UJ_ObjectiveManagerComponent::ActiveNextObjective);
		// 목표 수행도 갱신함수 바인드
		objectiveActor->sendObjSuccessDel.AddUObject(this, &UJ_ObjectiveManagerComponent::UpdateObjectiveSuccess);


		// 목표 액터 배열에 추가
		// 순서대로 하기위해 pinNo 사용
		objectiveDataAry[mData.pinNo].objectiveActor = objectiveActor;
		// objectiveAry.Add(objectiveActor);
	}

	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::White, FString::Printf(TEXT("목표 액터 추가 완료 개수 : %d"), objectiveDataAry.Num()));

	// @@ 바로 0번 미션 활성화 할지 고민
	// ActiveNextObjective();
}

void UJ_ObjectiveManagerComponent::ActiveObjectiveByIdx(int mIdx)
{
	auto* obj = objectiveDataAry[mIdx].objectiveActor;
	// 활성화
	obj->IS_OBJECTIVE_ACTIVE = true;
}

void UJ_ObjectiveManagerComponent::ActiveNextObjective()
{
	// // 맨 처음 예외처리
	// if(CUR_ACTIVE_MISSION_IDX >= 0)
	// {
	// 	// 목표 수행도 배열에 저장
	// 	objectiveDataAry[CUR_ACTIVE_MISSION_IDX].successPercent = successPercent;

	// 	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::White, FString::Printf(TEXT("%d 번 미션 수행도 : %.2f")
	// 	, CUR_ACTIVE_MISSION_IDX, successPercent));
	// }
	// 인덱스 증가
	CUR_ACTIVE_MISSION_IDX++;
	// 여기서 목표 다했으면 결산으로 넘어가짐 setcuractive~~
	if(isMissionComplete) return;
	// 해당 목표 활성화
	ActiveObjectiveByIdx(CUR_ACTIVE_MISSION_IDX);
}

void UJ_ObjectiveManagerComponent::SetCurActiveMissionIdx(int value)
{
	if(value < 0 || value > objectiveDataAry.Num())
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("idx value 문제"));
		return;
	}

    curActiveMissionIdx = value;
    if (CUR_ACTIVE_MISSION_IDX >= objectiveDataAry.Num())
    {
        GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::White, TEXT("목표 종료됨"));

        // 미션 결산
		MissionComplete();

		return;
    }

    // 활성 미션 액터 설정
    CUR_ACTIVE_MISSION = objectiveDataAry[value].objectiveActor;
}

void UJ_ObjectiveManagerComponent::MissionComplete()
{
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::White, TEXT("미션 종료 결산 화면으로"));

	isMissionComplete = true;
}

void UJ_ObjectiveManagerComponent::UpdateObjectiveSuccess(int mIdx, float successPercent)
{
	// mIdx 범위 체크
	if(mIdx < 0 || mIdx > objectiveDataAry.Num())
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("수행도 갱신 인덱스 오류"));

		return;
	}
	// 갱신
	objectiveDataAry[mIdx].successPercent = successPercent;
}