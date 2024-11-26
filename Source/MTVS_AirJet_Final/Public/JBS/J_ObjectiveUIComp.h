// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "JBS/J_Utility.h"
#include "J_ObjectiveUIComp.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPopupEndDelegate);

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
		public:
	__declspec(property(get = GetObjUI, put = SetObjUI)) class UJ_ObjectiveUI* OBJ_UI;
	class UJ_ObjectiveUI* GetObjUI()
	{
		return objUI;
	}
	void SetObjUI(class UJ_ObjectiveUI* value)
	{
		objUI = value;
	}
		protected:

	// 목표 UI 프리팹
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Classes")
	TSubclassOf<class UJ_ObjectiveUI> objUIPrefab;

public:
	// 팝업 비활성화 딜리게이트
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, BlueprintAssignable, Category="Default|Delegate")
	FPopupEndDelegate popupEndDel;
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
	UFUNCTION(Client, Unreliable)
    void CRPC_StartObjUI(const FTacticalOrderData& orderData);

    //XXX 목표 UI 갱신
	UFUNCTION(Client, Unreliable)
    void CRPC_UpdateObjUI(const FTacticalOrderData& orderData, bool isInit = false);

	// 목표 UI 시작 및 갱신
	UFUNCTION(Client, Unreliable)
    void CRPC_StartObjUIEngine(const FEngineProgressData& orderData);

	// 목표별 rpc 오버로딩 함수
	UFUNCTION(Client, Unreliable)
	void CRPC_UpdateObjUIEngine(const FEngineProgressData &orderData, bool isInit = false);

	// 목표 UI 시작 및 갱신
	UFUNCTION(Client, Unreliable)
	void CRPC_StartObjUITakeOff(const FTakeOffData &orderData);

        // 목표별 rpc 오버로딩 함수
	UFUNCTION(Client, Unreliable)
	void CRPC_UpdateObjUITakeOff(const FTakeOffData &orderData, bool isInit = false);

	// 목표 UI 시작 및 갱신
	UFUNCTION(Client, Unreliable)
	void CRPC_StartObjUIFormation(const FFormationFlightUIData &orderData);

        // 목표별 rpc 오버로딩 함수
	UFUNCTION(Client, Unreliable)
	void CRPC_UpdateObjUIFormation(const FFormationFlightUIData &orderData, bool isInit = false);

	// 목표 UI 시작 및 갱신
	UFUNCTION(Client, Unreliable)
	void CRPC_StartObjUINeut(const FNeutralizeTargetUIData &orderData);

        // 목표별 rpc 오버로딩 함수
	UFUNCTION(Client, Unreliable)
	void CRPC_UpdateObjUINeut(const FNeutralizeTargetUIData &orderData, bool isInit = false);

        // UFUNCTION(Client, Unreliable)
	// void CRPC_UpdateObjUI(const FFormationFlightUIData& orderData, bool isInit = false) {}

	// 서브 조건 완료 처리
	UFUNCTION(Client, Reliable)
    void CRPC_EndSubObjUI(int idx = 0, bool isSuccess = true);
	// 전술명령 완료 처리
	UFUNCTION(Client, Reliable)
	void CRPC_EndObjUI(bool isSuccess = true);

	// 설명 이미지 직접 변경
	UFUNCTION(Client, Unreliable)
	void CRPC_DirectSetDetailImg(const EMissionProcess &type);

        // 미션 결산 화면 전환 UI
	UFUNCTION(Client, Reliable)
	void CRPC_SwitchResultUI(const TArray<FObjectiveData>& resultObjData);

	// 미션 ai 피드백 데이터 전달
	UFUNCTION(Client, Reliable)
	void CRPC_SetResultAIFeedback(const FAIFeedbackRes &resData);

        // 팝업 ui 활성화 및 설정
	UFUNCTION(Client, Unreliable)
	void CRPC_ActivePopupUI(const EMissionProcess &mpIdx);

	// 팝업 ui 비활성화
	UFUNCTION(BlueprintCallable)
	void DeactivatePopupUI();

        // 팝업 ui 비활성화 됨 알림
	UFUNCTION(Server, Reliable)
	void SRPC_DeactivatedPopupUI();

        // 결산 UI 가져오기 | 바로 접근용
	UFUNCTION(BlueprintCallable)
	class UJ_MissionCompleteUI *GetMissionCompleteUI();
};
