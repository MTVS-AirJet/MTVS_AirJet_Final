// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Blueprint/UserWidget.h"
#include "CoreMinimal.h"
#include "J_Utility.h"
#include "J_BaseMissionObjective.generated.h"


DECLARE_MULTICAST_DELEGATE_TwoParams(FSendSuccessDelegate, AJ_BaseMissionObjective*, float);
DECLARE_MULTICAST_DELEGATE(FObjectiveEndDelegate);
DECLARE_MULTICAST_DELEGATE(FObjectiveActiveDelegate);
DECLARE_MULTICAST_DELEGATE(FObjectiveSuccessUpdateDelegate);

UCLASS()
class MTVS_AIRJET_FINAL_API AJ_BaseMissionObjective : public AActor
{
	GENERATED_BODY()
public:	
	AJ_BaseMissionObjective();
protected:
#pragma region 컴포넌트
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
#pragma endregion

#pragma region 값
	// 개별 pc 이전 UI 정보 맵 | 중복 ui update 방지용
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Values")
	TMap<class AJ_MissionPlayerController*, FTacticalOrderData> prevObjUIDataMap;

	// 전술명령 활성화 여부
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Values")
	bool isObjectiveActive = false;
		public:
	// getset
	__declspec(property(get = GetObjectiveActive, put = SetObjectiveActive)) bool IS_OBJECTIVE_ACTIVE;
	UFUNCTION(BlueprintCallable)
	virtual bool GetObjectiveActive() {return isObjectiveActive;}
	UFUNCTION(BlueprintCallable)
	virtual void SetObjectiveActive(bool value);
		protected:

	// 수행 성공도 0~1 값
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Values")
	float successPercent = 0.f;
		public:
	__declspec(property(get = GetSuccessPercent, put = SetSuccessPercent)) float SUCCESS_PERCENT;
	float GetSuccessPercent() {return successPercent;}
	void SetSuccessPercent(float value);
		protected:

	// 목표 완료했는지 여부
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Values")
	bool isObjEnded = false;
		public:
	__declspec(property(get = GetObjEnded, put = SetObjEnded)) bool IS_OBJ_ENDED;
	bool GetObjEnded() {return isObjEnded;}
	void SetObjEnded(bool value) {isObjEnded = value;}
		protected:
#pragma endregion

public:
	// 명령 종류
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Values")
	ETacticalOrder orderType = ETacticalOrder::NONE;

	// 명령에 따른 지휘관 보이스라인
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Values")
	FString commanderVoiceBase64;

#pragma region 미션 진행 딜리게이트 단
	/*
	미션 비활성화(생성) -> 미션 활성화(시작) -> 미션 수행도 갱신 -> 미션 완료(성공/실패) -> 미션 비활성화(종료) 순 호출됨
	*/
	// 미션 활성화 시 실행 딜리게이트
	FObjectiveActiveDelegate objectiveActiveDel;
	// 미션 비활성화 시 실행 딜리게이트
	FObjectiveActiveDelegate objectiveDeactiveDel;
	// 미션 완료 시 실행 딜리게이트
	FObjectiveEndDelegate objectiveEndDel;
	// 미션 성공 시 실행 딜리게이트
	FObjectiveEndDelegate objectiveSuccessDel;
	// 미션 실패 시 실행 딜리게이트
	FObjectiveEndDelegate objectiveFailDel;
	// 미션 수행도 갱신 딜리게이트
	FObjectiveSuccessUpdateDelegate objSuccessUpdateDel;
	// 미션 수행도 전송 딜리게이트
	FSendSuccessDelegate sendObjSuccessDel;
#pragma endregion

#pragma region 함수단
protected:
	virtual void BeginPlay() override;
	// 목표 활성화/비활성화
	UFUNCTION(BlueprintCallable)
	virtual void ObjectiveActive();
	UFUNCTION(BlueprintCallable)
	virtual void ObjectiveDeactive();

	// 목표 성공/실패
	UFUNCTION(BlueprintCallable)
    virtual void ObjectiveSuccess();
	UFUNCTION(BlueprintCallable)
    virtual void ObjectiveFail();

#pragma region UI 적용 요청 단
/* 목표 활성화 시 */
	
	// 목표 UI 시작 | 목표 활성화 시 호출
	UFUNCTION(BlueprintCallable)
	virtual void StartNewObjUI();
	

	
	// 동일한 UI data 보내는지 체크 | 과도한 crpc 방지책
	UFUNCTION(BlueprintCallable)
	virtual bool CheckSendSameData(class AJ_MissionPlayerController* pc, const FTacticalOrderData& uiData);

	// 목표 UI 완료 | 목표 완료시 호출
	UFUNCTION(BlueprintCallable)
	virtual void EndObjUI();
	// 개인 목표 서브 조건 UI 완료 
	UFUNCTION(BlueprintCallable)
	virtual void EndSubObjUI(class AJ_MissionPlayerController* pc, int idx = 0, bool isSuccess = true);
#pragma endregion
	// 배열내 pc 전부에게 해당 idx ai 지휘관 보이스 재생 요청
	UFUNCTION(BlueprintCallable)
	virtual void ReqPlayCommVoice(int idx, const TArray<class AJ_MissionPlayerController*>& pcs);

	// 아이콘 3d ui에 로컬 폰 설정
	void SetTargetIconUI();

	// 아이콘 ui에 거리 텍스트 설정
	void UpdateObjDisIconUI();

        // 리플리케이트
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const;
public:	
	virtual void Tick(float DeltaTime) override;
	// 미션 종료 처리
	UFUNCTION(BlueprintCallable)
	virtual void ObjectiveEnd(bool isSuccess = true);
	// 목표 값 설정
	virtual void InitObjective(ETacticalOrder type, bool initActive = false);

	
#pragma endregion

#pragma region 상속시 재정의 필수 함수 단
protected:
	UFUNCTION(BlueprintCallable)
	// 한 pc에게 목표 UI 데이터 보내기 | 상속하면 재정의
	virtual void SendObjUIData(class AJ_MissionPlayerController *pc, bool isInit = false);
	// 목표 UI 값 갱신 | 수행도 갱신 시 호출
	UFUNCTION(BlueprintCallable)
	virtual void UpdateObjUI();
	// 생성할 때 딜리게이트 바인드 공간
	virtual void InitBindDel();

    public:

#pragma endregion
};

