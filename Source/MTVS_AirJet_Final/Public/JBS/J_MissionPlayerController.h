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

protected:
	// 플레이어 역할
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
	EPlayerRole playerRole;
	// XXX 스폰 위치
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
	// FTransform spawnTR;


	

public:

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

    public:
	virtual void Tick(float deltaTime);

	UFUNCTION(BlueprintCallable)
	void SpawnMyPlayer();
	
};
