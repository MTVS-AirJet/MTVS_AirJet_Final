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
	FEngineProgressAllData allEngineProgData;

	// 이륙 준비 완료 체크
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Values")
	bool isReadyTakeOff = false;

public:

protected:
#pragma region 시작 단
/* begin -> active 엔진 수행 데이터 초기화, 수행 딜리게이트 바인드
-> 목표 종료시 수행도 계산 -> */
    virtual void BeginPlay() override;

    virtual void ObjectiveActive() override;

#pragma endregion
#pragma region 반복 단
/* viper 쪽에서 시동 절차 수행하면 딜리게이트 실행 | CheckProgress
-> 다음 수행으로 넘어가기 및 점수 추가 | ActiveNextProgress, CalcSuccessPercent
-> 모두 대기 절차 면 넘어가기 & 이륙 절차면 목표 종료 | CheckAllRunEngine*/
    // 수행 절차 성공 체크
    UFUNCTION(BlueprintCallable)
    void CheckProgress(class AJ_MissionPlayerController *pc, EEngineProgress type);

    // 해당 pc 수행 절차 성공 처리
    void ActiveNextProgress(FEngineProgressData& data, bool isSuccess = true);

	// 목표 성공시 수행도 계산
	UFUNCTION(BlueprintCallable)
    void CalcSuccessPercent();

	// 전체 시동 완료 체크
    bool CheckAllRunEngine(const TArray<class AJ_MissionPlayerController *> pcs, EEngineProgress checkType);
#pragma endregion

#pragma region obj UI 설정 단
	FEngineProgressData SetEngineUIData(class AJ_MissionPlayerController *pc = nullptr);

    virtual void SendObjUIData(class AJ_MissionPlayerController *pc, bool isInit = false) override;

    virtual void UpdateObjUI() override;
#pragma endregion

public:
    virtual void Tick(float deltaTime) override;
};
