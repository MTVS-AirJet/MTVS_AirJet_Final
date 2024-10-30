// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KHS/K_PlayerController.h"
#include <JBS/J_Utility.h>
#include "J_MissionPlayerController.generated.h"

/**
 * 
 */
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

public:
	// 목표 UI 관리 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Component")
	class UJ_ObjectiveUIComponent* objUIComp;

	// 파일럿 역할
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Values", Replicated)
	EPilotRole pilotRole;

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
	UFUNCTION(Client, Reliable)
	void CRPC_RemoveLoadingUI();

	// 시작 지점 텔레포트
	UFUNCTION(NetMulticast, Reliable)
	void MRPC_TeleportStartPoint(FTransform tpTR);

        // LHJ 추가
	UPROPERTY(EditDefaultsOnly , Category="UI")
	TSubclassOf<class UUserWidget> WaitingForStartFac;
};
