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
    TMap<class AJ_MissionPlayerController*, FTakeOffCheckData> takeOffCheckMap;
	
    // 실패 판정 기준 방향
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|FailCheck|Values")
    FVector baseDirection;

	// 실패 판정 거리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|FailCheck|Values")
	float failDis = 1000.f;

    // 실패 판정 체크 간격
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|FailCheck|Values")
    float failCheckInterval = .5f;

	FTimerHandle checkTimeHandle;

    // 현재 이륙 수행 퍼센트
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
    float curFlightPercent = 0;

public:

protected:
#pragma region 시작 단
/* active -> 게임모드 이륙시 딜리게이트에 이륙 체크 함수 바인드 | AddFlightedPC -> SuccessTakeOff
-> 목표 위치 텔포 박스로 설정 | SetPosition
-> 실패 체크 기준 방향 설정 | CalcBaseDirection*/

    virtual void BeginPlay() override;

    // 이륙 목표 활성화
    virtual void ObjectiveActive() override;

    // 위치 설정 | 텔포 박스위치, 편대장 바라보기
    void SetPosition(class AK_CesiumTeleportBox *tpBox);

    // 기준 방향 설정 | 편대장 기준
    FVector CalcBaseDirection();

    // 바인드
    virtual void InitBindDel();
#pragma endregion

#pragma region 이륙 단
/* 한 명 이륙 시 이륙 성공 딜리게이트 실행 | SuccessTakeOff
-> 이륙 점수 갱신 | CalcSuccessPercent
-> 전부 이륙시 매니저 쪽에서 완료 처리 | ObjectiveEnd*/

    // 해당 파일럿 이륙 성공 처리
    void SuccessTakeOff(class AJ_MissionPlayerController *pc, bool isSuccess = true);

    // 목표 성공시 수행도 계산
    UFUNCTION(BlueprintCallable)
    void CalcSuccessPercent();

    virtual void ObjectiveEnd(bool isSuccess = true) override;

#pragma endregion

#pragma region 반복 단
/* begin에서 실패 판정 타이머 반복 | CheckFail
-> 
*/
    // 실패 체크
    void CheckFail();
#pragma endregion

#pragma region 목표 UI 설정 단
/* 특정 타이밍에 목표 UI 시작 | StartNewObjUI
-> 각 pc마다 목표 ui 데이터 설정 | SendObjUIData, SetTakeOffUIData
-> 특정 갱신 타이밍 마다 업데이트 UI | UpdateObjUI -> 이전에 보냈는지 체크후 업데이트 | CheckSendSameData
-> 목표 종료 나 서브 목표 종료시 ui 애니메이션 실행 요청 | EndObjUI , EndSubObjUI
-> 목표 종료시 비작동*/

    virtual void SendObjUIData(class AJ_MissionPlayerController *pc, bool isInit = false);

    // 목표 ui에 표시할 데이터 설정
    virtual FTakeOffData SetTakeOffUIData(class AJ_MissionPlayerController *pc = nullptr);

    // 목표 UI 값 갱신 | 수행도 갱신 시 호출
    virtual void UpdateObjUI() override;

#pragma endregion

public:
    virtual void Tick(float deltaTime);
};
