// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KHS/K_PlayerController.h"
#include <JBS/J_Utility.h>
#include "J_MissionPlayerController.generated.h"

/**
 * 
 */
DECLARE_DELEGATE_TwoParams(FSendEngineProgressSuccessDel, AJ_MissionPlayerController*, EEngineProgress);

UCLASS()
class MTVS_AIRJET_FINAL_API AJ_MissionPlayerController : public AK_PlayerController
{
	GENERATED_BODY()
public:
	AJ_MissionPlayerController();

protected:
	// XXX 플레이어 역할
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
	EPlayerRole playerRole;

	// 디버그용 인풋 사용 유무
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Debug")
	bool enableDebugInput = false;

	// 로딩 ui
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|UI")
	class UK_LoadingWidget* missionReadyUI = nullptr;
	// 로딩 ui 프리팹
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Classes")
	TSubclassOf<class UK_LoadingWidget> loadingUIPrefab;

	// 로딩 ui 제거용 타이머
	FTimerHandle removeLoadingUITH;

	// 로딩 ui 제거 햇음
	bool wasRemovedUI = false;

	// 미션 보이스 데이터 맵
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Values")
	TMap<int, class USoundWaveProcedural*> missionVoiceMap;

	// 목표 사운드 데이터 맵
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
	TMap<EObjSound, class USoundBase*> objSoundMap;

	// 전체 보이스 요청 허용 | 낭비 방지
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Debug")
	bool enableReqAllVoice = true;


public:
	// 목표 UI 관리 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Component")
	class UJ_ObjectiveUIComp* objUIComp;

	// 지휘관 보이스 라인 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Component")
	class UAudioComponent* commanderAudioComp;

	// 목표 액터용 사운드 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Component")
	class UAudioComponent* objAudioComp;

	// 파일럿 역할
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Values", Replicated)
	EPilotRole pilotRole;

	// 엔진 수행 성공 딜리게이트
	FSendEngineProgressSuccessDel sendEngineProgDel;

protected:
    virtual void BeginPlay() override;
    virtual void OnPossess(APawn *newPawn);

	// gi에서 프리팹 가져와서 플레이어 스폰
    UFUNCTION(Server, Reliable)
    void SRPC_SpawnMyPlayer(TSubclassOf<class APawn> playerPrefab);

	// 스폰한 플레이어 포제스
	UFUNCTION(Client, Reliable)
	void CRPC_SpawnMyPlayer(APawn *newPawn);

	// XXX streaming ui 생성 | 의존성 제거됨
	void InitStreamingUI(class AJ_BaseMissionPawn *newPawn);

	// 

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;

public:
	virtual void Tick(float deltaTime);

	UFUNCTION(BlueprintCallable)
	void SpawnMyPlayer();

	// 이륙 pc 배열에 자신 추가
	UFUNCTION(Server, Reliable)
	void SRPC_AddFlightArySelf();

	// 로딩 ui 추가
	UFUNCTION(Client, Reliable)
	void CRPC_AddLoadingUI();

	// 로딩 ui 제거
	UFUNCTION(Server, Reliable)
	void SRPC_RemoveLoadingUI();

	// 로딩 ui 제거
	UFUNCTION(BlueprintCallable, Client, Reliable)
	void CRPC_RemoveLoadingUI();

	// 시작 지점 텔레포트
	UFUNCTION(NetMulticast, Reliable)
	void MRPC_TeleportStartPoint(FTransform tpTR);

	// 시동 수행 알리기 | 서버단
	UFUNCTION(Server, Reliable)
	void SRPC_SendEngineProgressSuccess(EEngineProgress type);

	// 포제스 시 로컬 클라에서 작동
	UFUNCTION(Client, Reliable)
	void CRPC_OnPossess();

	// 목표 시작시 지휘관 보이스 라인 재생
	UFUNCTION(Client, Reliable)
	void CRPC_PlayCommanderVoice(const FString &voiceBase64);

	// 목표 시작시 지휘관 보이스 라인 재생
	UFUNCTION(Client, Reliable)
	void CRPC_PlayCommanderVoice2(const ETacticalOrder &orderType);



	// 보이스라인 재생
	UFUNCTION(BlueprintCallable)
	void PlayVoiceByIdx(const int &idx);

        UFUNCTION(Client, Reliable)
	void CRPC_PlayCommanderVoice3(int idx);

	UFUNCTION(Client, Unreliable)
	void CRPC_PlayVoiceByMP(const EMissionProcess &missionProcess);





	// 보이스 라인 재생
	UFUNCTION(BlueprintCallable)
	void PlayCommanderVoice3(const FCommanderVoiceRes &resData);

	// 미션 보이스 데이터 요청
	UFUNCTION(Client, Unreliable)
	void CRPC_ReqMissionVoiceData();

        // 보이스 데이터 받아서 맵에 저장
	UFUNCTION(BlueprintCallable)
	void ResMissionVoiceData(const FAllVoiceRes &resData);

	// 목표 소리 재생
	UFUNCTION(Client, Unreliable)
	void CRPC_PlayObjSound(const EObjSound &idx);

	// 미션 종료 전 동기화 타이머 해제
	UFUNCTION(BlueprintCallable)
	void ClearSyncTimer();

#pragma region LHJ 추가
	UPROPERTY(EditDefaultsOnly , Category="UI")
	TSubclassOf<class UUserWidget> WaitingForStartFac;
#pragma endregion
};
