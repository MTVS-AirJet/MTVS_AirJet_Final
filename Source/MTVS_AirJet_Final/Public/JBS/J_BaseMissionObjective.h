// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Blueprint/UserWidget.h"
#include "CoreMinimal.h"
#include "J_Utility.h"
#include "J_BaseMissionObjective.generated.h"


DECLARE_MULTICAST_DELEGATE_TwoParams(FSendSuccessDelegate, AJ_BaseMissionObjective*, float)
DECLARE_MULTICAST_DELEGATE(FObjectiveEndDelegate)
DECLARE_MULTICAST_DELEGATE(FObjectiveActiveDelegate)
DECLARE_MULTICAST_DELEGATE(FObjectiveSuccessUpdateDelegate)

UCLASS()
class MTVS_AIRJET_FINAL_API AJ_BaseMissionObjective : public AActor
{
	GENERATED_BODY()
public:	
	AJ_BaseMissionObjective();

protected:
	virtual void BeginPlay() override;
public:	
	virtual void Tick(float DeltaTime) override;

protected:
	// 미션 액터 루트
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Component")
	class USceneComponent* rootComp;

	// 위치 표기용 메시 & 화살표
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Component")
	class UStaticMeshComponent* sphereComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Component")
	class UArrowComponent* forWComp;

	// 아이콘 3d ui 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Component")
	class UJ_CustomWidgetComponent* iconWorldUIComp;
	// 아이콘 UI
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|UI")
	class UJ_ObjectiveIconUI* iconWorldUI;

	// 전술명령 활성화 여부
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Values", ReplicatedUsing=OnRep_ObjActive)
	bool isObjectiveActive = false;
		public:
	__declspec(property(get = GetObjectiveActive, put = SetObjectiveActive)) bool IS_OBJECTIVE_ACTIVE;
	UFUNCTION(BlueprintCallable)
	virtual bool GetObjectiveActive()
	{
		return isObjectiveActive;
	}
	UFUNCTION(BlueprintCallable)
	virtual void SetObjectiveActive(bool value);

	UFUNCTION(BlueprintCallable)
	virtual void OnRep_ObjActive();

    protected:

	// 수행 성공도 0~1 값
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Values")
	float successPercent = 0.f;
		public:
	__declspec(property(get = GetSuccessPercent, put = SetSuccessPercent)) float SUCCESS_PERCENT;
	float GetSuccessPercent()
	{
		return successPercent;
	}
	void SetSuccessPercent(float value);

		protected:

	// 모든 pc에 대한 ui 정보 배열
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Values")
	TMap<class AJ_MissionPlayerController*, FTacticalOrderData> prevObjUIDataMap;



    public:
	// 명령 종류
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Values")
	ETacticalOrder orderType = ETacticalOrder::NONE;

	/*
	미션 비활성화(생성) -> 미션 활성화(시작) -> 미션 수행도 갱신 -> 미션 완료(성공/실패) -> 미션 비활성화(종료) 순 호출됨
	*/

	// 미션 완료 시 실행 딜리게이트
	FObjectiveEndDelegate objectiveEndDel;
	// 미션 성공 시 실행 딜리게이트
	FObjectiveEndDelegate objectiveSuccessDel;
	// 미션 실패 시 실행 딜리게이트
	FObjectiveEndDelegate objectiveFailDel;

	// 미션 활성화 시 실행 딜리게이트
	FObjectiveActiveDelegate objectiveActiveDel;

	// 미션 비활성화 시 실행 딜리게이트
	FObjectiveActiveDelegate objectiveDeactiveDel;

	// 미션 수행도 갱신 딜리게이트
	FObjectiveSuccessUpdateDelegate objSuccessUpdateDel;
	FSendSuccessDelegate sendObjSuccessDel;
protected:
	// 목표 성공/실패
	UFUNCTION(BlueprintCallable)
    virtual void ObjectiveSuccess();
	UFUNCTION(BlueprintCallable)
    virtual void ObjectiveFail();

	// 목표 활성화/비활성화
	UFUNCTION(BlueprintCallable)
	virtual void ObjectiveActive();
	UFUNCTION(BlueprintCallable)
	virtual void ObjectiveDeactive();

	// 목표 시점에 따라 모든 PC 에게 호출
	// 서버에서 실행
	// 목표 UI 시작 | 목표 활성화 시 호출
	UFUNCTION(Server, Reliable)
	virtual void SRPC_StartNewObjUI();

	// 개인 목표 ui에 표시할 데이터 만들기 | 상속하면 재정의
	UFUNCTION(BlueprintCallable)
	virtual FTacticalOrderData SetObjUIData(class AJ_MissionPlayerController* pc = nullptr);

	// 목표 UI 값 갱신 | 수행도 갱신 시 호출
	UFUNCTION(Server, Reliable)
	virtual void SRPC_UpdateObjUI();
	// 목표 UI 완료 | 목표 완료시 호출
	UFUNCTION(Server, Reliable)
	virtual void SRPC_EndObjUI();
	// 개인 목표 서브 조건 UI 완료 
	UFUNCTION(Server, Reliable)
	virtual void SRPC_EndSubObjUI(class AJ_MissionPlayerController* pc, int idx = 0, bool isSuccess = true);
	// @@ 모든 pc에서 서브 조건 완료 처리

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const;

    public:
	// 미션 종료 처리
	UFUNCTION(BlueprintCallable)
	virtual void ObjectiveEnd(bool isSuccess = true);

	// 목표 값 설정 | 하위 전술명령에서 재정의 필요
	virtual void InitObjective(ETacticalOrder type, bool initActive = false);
};
