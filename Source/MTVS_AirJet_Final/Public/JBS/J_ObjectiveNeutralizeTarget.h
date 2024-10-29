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

	// 생성 개수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
	int spawnTargetAmt = 1;

	// 스폰 지점
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Values")
	FTransform spawnTR;

	// 파괴 카운트
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Values")
	int destroyedTargetAmt = 0;

	
public:

protected:
	//
    virtual void BeginPlay() override;
    // 스폰 지점 계산
    bool CalcSpawnTransform(FTransform& outSpawnTR);
    // 지상 타겟 스폰
    void SpawnGroundTarget();
	// 타겟 파괴 카운트
	UFUNCTION(BlueprintCallable)
	void CountTargetDestroyed();

	// 목표 UI 시작 | 목표 활성화 시 호출
	virtual void SRPC_StartNewObjUI() override;
	// 목표 UI 값 갱신 | 수행도 갱신 시 호출
	virtual void SRPC_UpdateObjUI() override;
	// 목표 UI 완료 | 목표 완료시 호출
	virtual void SRPC_EndObjUI() override;
	// 목표 서브 조건 UI 완료
	virtual void SRPC_EndSubObjUI() override;

    public:
	virtual void Tick(float deltaTime) override;
};
