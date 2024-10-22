// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "JBS/J_BaseMissionObjective.h"
#include "J_NeutralizeTarget.generated.h"

/**
 * 
 */
UCLASS()
class MTVS_AIRJET_FINAL_API AJ_NeutralizeTarget : public AJ_BaseMissionObjective
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
	FTransform spawnTR;

	// 파괴 카운트
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
	int destroyedTargetAmt = 0;
public:

protected:
	//
    virtual void BeginPlay() override;
    // 스폰 지점 계산
    FTransform CalcSpawnTransform();
    // 지상 타겟 스폰
    void SpawnGroundTarget();
	// 타겟 파괴 카운트
	UFUNCTION(BlueprintCallable)
	void CountTargetDestroyed();

    public:
	virtual void Tick(float deltaTime) override;
};
