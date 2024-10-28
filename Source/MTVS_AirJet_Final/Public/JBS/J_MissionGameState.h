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

	// 스트리밍 유저 id 추가 | ServerRPC_SetStreamingPlayer_Implementation 에서 하던것 여기로 이동함
    void AddStreamUserId(const FString &userId);
    // 스트리밍 유저 id 제거 | ServerRPC_SetStreamingPlayer_Implementation 에서 하던것 여기로 이동함
    void RemoveStreamUserId(const FString &userId);

    // 현재 레벨 모든 pc 가져오기
    virtual TArray<class AJ_MissionPlayerController *> GetAllPlayerController();

    // @@ 아마 나중에 캐스팅 필요할지도
    // 현재 레벨 모든 플레이어 폰 가져오기
    virtual TArray<class APawn *> GetAllPlayerPawn();
};
