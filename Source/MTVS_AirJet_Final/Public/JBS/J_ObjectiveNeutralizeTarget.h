// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "JBS/J_BaseMissionObjective.h"
#include "J_ObjectiveNeutralizeTarget.generated.h"

/**
 * 
 */


UCLASS()
class MTVS_AIRJET_FINAL_API AJ_ObjectiveNeutralizeTarget : public AJ_BaseMissionObjective
{
	GENERATED_BODY()
protected:
	// 지상 타겟 프리팹
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Classes")
	TSubclassOf<class AJ_GroundTarget> groundTargetPrefab;

	// XXX 생성 개수 | 과녁으로 변경해서 사실상 고정
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
	int spawnTargetAmt = 1;

	// 스폰 지점
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Values")
	FTransform spawnTR;

	// XXX 파괴 카운트
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Values")
	int destroyedTargetAmt = 0;

	// 이동 목표 프리팹
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Classes")
	TSubclassOf<class AJ_ObjectiveMovePoint> movePointPrefab; 

	// 하위 이동 목표 배열
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Objects")
	TArray<class AJ_ObjectiveMovePoint*> subMPArray;

	// 서브 이동 목표 개수 == 4
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
	int subMovePointAmt = 4;

	// 현재 진행 중인 서브 목표 인덱스
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Values")
	int curActiveSubMPIdx = -1;
		public:
	__declspec(property(get = GetCurSubMPIdx, put = SetCurSubMPIdx)) int CUR_ACTIVE_SUBMP_IDX;
	int GetCurSubMPIdx()
	{
		return curActiveSubMPIdx;
	}
	void SetCurSubMPIdx(int value)
	{
		curActiveSubMPIdx = value;
	}
		protected:

	// 과녁 -> 0번 기둥 거리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
	float firstMPDis = 6825;
	// 0->1 , 2->3 세로 기둥 거리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
	float yMPDis = 9150;
	// 1->2 가로 기둥 거리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
	float xMPDis = 27300;


	// 과녁 점수 맵
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Values")
	TMap<class AJ_MissionPlayerController*, float> targetScoreMap;
	
	// 전체 pc
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Objects")
	TArray<class AJ_MissionPlayerController*> allPC;
	

public:

protected:
	//
    virtual void BeginPlay() override;
    // 스폰 지점 계산
    bool CalcSpawnTransform(FTransform& outSpawnTR);
    // 지상 타겟 스폰
	UFUNCTION(BlueprintCallable)
    void SpawnGroundTarget();
	// 타겟 파괴 카운트
	UFUNCTION(BlueprintCallable)
	void CountTargetDestroyed();

	// 목표 UI에 표시할 데이터 구성
	virtual FTacticalOrderData SetObjUIData(class AJ_MissionPlayerController *pc = nullptr) override;

        // 목표 UI 시작 | 목표 활성화 시 호출
	virtual void SRPC_StartNewObjUI() override;
	// 목표 UI 값 갱신 | 수행도 갱신 시 호출
	virtual void SRPC_UpdateObjUI() override;
	// 목표 UI 완료 | 목표 완료시 호출
	virtual void SRPC_EndObjUI() override;
	// 목표 서브 조건 UI 완료
	// virtual void SRPC_EndSubObjUI() override;

	// 서브 하위 목표들 생성
	UFUNCTION(BlueprintCallable)
	void InitSubMovePoints();

	// 해당 인덱스 서브 목표 활성화
	void ActiveObjectiveByIdx(int mIdx, bool isFirst = false);

	// 지연 시간 후 서브 목표 활성화
	void DelayedObjectiveActive(AJ_BaseMissionObjective *obj, float delayTime);

	// 과녁 점수 갱신
	void UpdateTargetScore(class AJ_MissionPlayerController *pc, float score);

        // 서브 이동 목표 위치 구하기 | 기준 위치, 인덱스
	FTransform CalcSubMPTransform(const FTransform &baseTR, const int &idx);

public:
	virtual void Tick(float deltaTime) override;

	// 다음 서브 목표 활성화
	void ActiveNextObjective();
};
