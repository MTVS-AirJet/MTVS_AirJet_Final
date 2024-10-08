// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "J_MissionGamemode.generated.h"

/**
 * 
 */
UCLASS()
class MTVS_AIRJET_FINAL_API AJ_MissionGamemode : public AGameModeBase
{
	GENERATED_BODY()

protected:
    // 미션 지역 맵
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Classes")
	TSoftObjectPtr<UWorld> missionMapPrefab;

    // // 지통실 맵
    // UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Classes")
    // TSoftObjectPtr<UWorld> commandMapPrefab;

public:

protected:
    virtual void BeginPlay() override;

    void LoadMissionMap();

public:
    virtual void Tick(float DeltaSeconds) override;
};
