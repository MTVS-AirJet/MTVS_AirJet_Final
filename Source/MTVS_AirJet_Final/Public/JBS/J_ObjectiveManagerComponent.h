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

	// 시동/이륙 데이터 리스트
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Objects")
	TArray<FObjectiveData> defaultObjDataAry = {
		FObjectiveData(ETacticalOrder::ENGINE_START)
		,FObjectiveData(ETacticalOrder::TAKE_OFF)
	};

	// 미션 목표 데이터 리스트
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Objects")
	TArray<FObjectiveData> objectiveDataAry;

	// 현재 진행 중인 미션 | Set 은 인덱스를 통하기
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Objects")
	class AJ_BaseMissionObjective* curActiveMission = nullptr;
		public:
	__declspec(property(get = GetCurActiveMission, put = SetCurActiveMission)) class AJ_BaseMissionObjective* CUR_ACTIVE_MISSION;
	class AJ_BaseMissionObjective* GetCurActiveMission()
	{
		return curActiveMission;
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
	void SetCurActiveMissionIdx(int value);

    protected:


	// 미션 클리어
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
	bool isMissionComplete = false;

	// 목표 액터 프리팹 맵
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Classes")
	TMap<ETacticalOrder, TSubclassOf<class AJ_BaseMissionObjective>> objectiveActorPrefabMap;

	// 현재 활성화된 미션 액터 디버그
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Debug")
	bool enablePrintCurActiveMissionActor = false;

public:
	// @@ 목표 전환 대기 시간 | 잘 처리 할 방법 궁리 필요
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
	float objSwitchInterval = 1.5f;

protected:
	// 미션 클리어
	UFUNCTION(BlueprintCallable)
	void MissionComplete();

	// 목표 액터 생성 및 설정
	class AJ_BaseMissionObjective* SpawnObjActor(ETacticalOrder type, const FTransform &spawnTR = FTransform());

	// 미션 시작시 기본 목표(시동/이륙) 종료 처리
	UFUNCTION(BlueprintCallable)
	void SkipDefaultObj(bool isSuccess);

public:
	// 레벨 시작 시 시동/이륙 목표 설정
    void InitDefaultObj();
    // 미션 시작 시 목표 리스트 설정
    void InitObjectiveList(TArray<struct FMissionObject> missions);

	// 해당 목표 활성화
    void ActiveObjectiveByIdx(int mIdx, bool isFirst = false);

	// 딜레이된 목표 활성화
    void DelayedObjectiveActive(class AJ_BaseMissionObjective *obj, float delayTime);

    // 다음 목표 활성화 | 목표에 바인드
    void ActiveNextObjective();

	// 목표 수행도 갱신
	UFUNCTION(BlueprintCallable)
	void UpdateObjectiveSuccess(class AJ_BaseMissionObjective* objActor, float successPercent);

	// 기본 목표 시작
	void StartDefualtObj();
};
