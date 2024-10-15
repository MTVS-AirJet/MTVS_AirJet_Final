// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KHS/K_GameState.h"
#include "J_MissionGameState.generated.h"

/**
 * 
 */
UCLASS()
class MTVS_AIRJET_FINAL_API AJ_MissionGameState : public AK_GameState
{
	GENERATED_BODY()
protected:


public:

protected:


public:
	// 화면 공유 플레이어 목록 UI 생성하는 부분 재정의
    virtual void OnRep_StreamingID() override;
};
