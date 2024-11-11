// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "JBS/J_Utility.h"
#include "J_ObjectiveUIComp.generated.h"


UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MTVS_AIRJET_FINAL_API UJ_ObjectiveUIComp : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UJ_ObjectiveUIComp();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	// 목표 UI
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|UI")
	class UJ_ObjectiveUI* objUI;

	// 목표 UI 프리팹
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Classes")
	TSubclassOf<class UJ_ObjectiveUI> objUIPrefab;

public:

protected:
	// 목표 UI 생성
    void InitObjUI();

	// 편대 비행 UI 값 생성
    void CreateUIData(const FFormationFlightUIData &data, TArray<FTextUIData>& outData, bool isInit = false);
	// 지대공 훈련
    void CreateUIData(const FNeutralizeTargetUIData &data, TArray<FTextUIData> &outData, bool isInit = false);
	// 시동 절차
    void CreateUIData(const FEngineProgressData &data, TArray<FTextUIData> &outData, bool isInit = false);
	// 이륙 절차
    void CreateUIData(const FTakeOffData &data, TArray<FTextUIData> &outData, bool isInit = false);

public:
	// 목표 UI 시작 및 갱신
	UFUNCTION(Client, Reliable)
    void CRPC_StartObjUI(const FTacticalOrderData& orderData);

    // 목표 UI 갱신
	// 지상 타겟 무력화
	UFUNCTION(Client, Reliable)
    void CRPC_UpdateObjUI(const FTacticalOrderData& orderData, bool isInit = false);

	// 서브 조건 완료 처리
	UFUNCTION(Client, Reliable)
    void CRPC_EndSubObjUI(int idx = 0, bool isSuccess = true);
	// 전술명령 완료 처리
	UFUNCTION(Client, Reliable)
	void CRPC_EndObjUI(bool isSuccess = true);

	// 미션 결산 화면 전환 UI
	UFUNCTION(Client, Reliable)
	void CRPC_SwitchResultUI(const TArray<FObjectiveData>& resultObjData);

	// 결산 UI 가져오기 | 바로 접근용
	UFUNCTION(BlueprintCallable)
	class UJ_MissionCompleteUI *GetMissionCompleteUI();
};
