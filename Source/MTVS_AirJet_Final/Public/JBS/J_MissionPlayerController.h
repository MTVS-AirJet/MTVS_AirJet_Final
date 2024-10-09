// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include <JBS/J_Utility.h>
#include "J_MissionPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class MTVS_AIRJET_FINAL_API AJ_MissionPlayerController : public APlayerController
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

	UFUNCTION(Server, Reliable)
	void SRPC_SpawnMyPlayer(TSubclassOf<class APawn> playerPrefab);

public:
	virtual void Tick(float deltaTime);

	
};
