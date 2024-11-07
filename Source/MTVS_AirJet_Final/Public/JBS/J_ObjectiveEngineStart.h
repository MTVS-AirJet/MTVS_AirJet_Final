// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "JBS/J_BaseMissionObjective.h"
#include "JBS/J_Utility.h"
#include "J_ObjectiveEngineStart.generated.h"

/**
 * 
 */
UCLASS()
class MTVS_AIRJET_FINAL_API AJ_ObjectiveEngineStart : public AJ_BaseMissionObjective
{
	GENERATED_BODY()
	/*
		모든 플레이어에 대해 수행 절차를 점검해야함
		따라서 모든 pc를 가져와서 각 pc 에 대해 수행 절차 체크와 ui 표시 값을 각각 따로 해주어야함
	*/
protected:
	// 시동 수행 전체 데이터
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Values")
	FEngineProgressAllData allData;

	// 활성화 당시 모든 pc
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Objects")
	TArray<class AJ_MissionPlayerController*> allPC;

public:

protected:
    virtual void BeginPlay() override;

	// 시동 목표 활성화
    virtual void ObjectiveActive() override;

	// 수행 절차 성공 체크
	UFUNCTION(BlueprintCallable)
    void CheckProgress(class AJ_MissionPlayerController *pc, EEngineProgress type);

	// 전체 시동 완료 체크
    bool CheckAllRunEngine(const TArray<class AJ_MissionPlayerController *> pcs, EEngineProgress checkType);

    // 해당 pc 수행 절차 성공 처리
    void ActiveNextProgress(FEngineProgressData& data, bool isSuccess = true);

	// 목표 성공시 수행도 계산
	UFUNCTION(BlueprintCallable)
    void CalcSuccessPercent();

	// 목표 ui에 표시할 데이터 설정
    virtual FTacticalOrderData SetObjUIData(class AJ_MissionPlayerController *pc = nullptr) override;

public:
    virtual void Tick(float deltaTime) override;
};
