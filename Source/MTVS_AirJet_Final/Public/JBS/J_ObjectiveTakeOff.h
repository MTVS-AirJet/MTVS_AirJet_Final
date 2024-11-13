// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "JBS/J_BaseMissionObjective.h"
#include "J_ObjectiveTakeOff.generated.h"

/**
 * 
 */
UCLASS()
class MTVS_AIRJET_FINAL_API AJ_ObjectiveTakeOff : public AJ_BaseMissionObjective
{
	GENERATED_BODY()
protected:
	// 파일럿 이륙 수행 여부 배열 | pc , 이륙 여부, 이륙 성공 여부
	TMap<class AJ_MissionPlayerController*, TPair<bool, bool>> takeOffCheckMap;
	
	// 활성화 당시 모든 pc
	TArray<class AJ_MissionPlayerController*> allPC;
	// 활성화 당시 모든 폰
	TArray<class APawn*> allPawn;

    // 실패 판정 기준 방향
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Values")
    FVector baseDirection;

	// 실패 판정 거리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
	float failDis = 1000.f;

    // 실패 판정 체크 간격
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
    float failCheckInterval = .5f;

	FTimerHandle checkTimeHandle;

    // 현재 이륙 수행 퍼센트
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
    float curFlightPercent = 0;

public:

protected:
    virtual void BeginPlay() override;
    // 이륙 목표 활성화
    virtual void ObjectiveActive() override;
    // 해당 파일럿 이륙 성공 처리
    void SuccessTakeOff(class AJ_MissionPlayerController *pc, bool isSuccess = true);
	// 위치 설정 | 텔포 박스위치, 편대장 바라보기
    void SetPosition(class AK_CesiumTeleportBox *tpBox);

    virtual void ObjectiveEnd(bool isSuccess = true) override;

    // 목표 UI 시작 | 목표 활성화 시 호출
    virtual void SRPC_StartNewObjUI() override;

    // 목표 UI 값 갱신 | 수행도 갱신 시 호출
    virtual void SRPC_UpdateObjUI() override;

    // 목표 성공시 수행도 계산
    UFUNCTION(BlueprintCallable)
    void CalcSuccessPercent();

    // 목표 ui에 표시할 데이터 설정
    virtual FTacticalOrderData SetObjUIData(class AJ_MissionPlayerController *pc = nullptr) override;

    // 목표 ui에 표시할 데이터 설정
    virtual FTakeOffData SetTakeOffUIData(class AJ_MissionPlayerController *pc = nullptr);

    // 실패 체크
    void CheckFail();

public:
    virtual void Tick(float deltaTime);
};
