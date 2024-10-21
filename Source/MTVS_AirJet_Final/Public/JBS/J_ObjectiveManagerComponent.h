// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "JBS/J_Utility.h"
#include "Templates/Tuple.h"
#include "J_ObjectiveManagerComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MTVS_AIRJET_FINAL_API UJ_ObjectiveManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UJ_ObjectiveManagerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	// 주인 게임모드 액터
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Objects")
	class AJ_MissionGamemode* ownerGM;

	// 미션 목표 리스트
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Objects")
	TArray<class AJ_BaseMissionObjective*> objectiveAry;

	// 현재 진행 중인 미션 | Set 은 인덱스를 통하기
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Objects")
	class AJ_BaseMissionObjective* curActiveMission = nullptr;
		public:
	__declspec(property(get = GetCurActiveMission, put = SetCurActiveMission)) class AJ_BaseMissionObjective* CUR_ACTIVE_MISSION;
	class AJ_BaseMissionObjective* GetCurActiveMission()
	{
		return objectiveAry[curActiveMissionIdx];
	}
	void SetCurActiveMission(class AJ_BaseMissionObjective* value)
	{
		curActiveMission = value;
	}
		protected:
	// 현재 진행 중인 인덱스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
	int curActiveMissionIdx = -1;
		public:
	__declspec(property(get = GetCurActiveMissionIdx, put = SetCurActiveMissionIdx)) int CUR_ACTIVE_MISSION_IDX;
	int GetCurActiveMissionIdx()
	{
		return curActiveMissionIdx;
	}
	void SetCurActiveMissionIdx(int value)
	{
		curActiveMissionIdx = value;
		// 활성 미션 액터 설정
		CUR_ACTIVE_MISSION = objectiveAry[value];
	}
		protected:

	// 목표 액터 프리팹 맵
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Classes")
	TMap<ETacticalOrder, TSubclassOf<class AJ_BaseMissionObjective>> objectiveActorPrefabMap;
public:
protected:


public:
	// @@ 게임모드에서 데이터 얻고 호출 해야함
	// 미션 시작 시 목표 리스트 설정
    void InitObjectiveList(TArray<struct FMissionObject> missions);

	// 해당 미션 활성화
    void ActiveObjectiveByIdx(int mIdx);

	// 다음 미션 활성화 | 미션에 바인드
    void ActiveNextObjective();
};
