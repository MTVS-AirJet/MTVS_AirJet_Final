﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "K_GameState.generated.h"

/**
 * 
 */
UCLASS()
class MTVS_AIRJET_FINAL_API AK_GameState : public AGameStateBase
{
	GENERATED_BODY()
	
private:
	// 네트워크 복제 설정 함수 ------------------------------------------------------------------------------
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


public:
   //=================화면공유 관련 정보 저장
   //Streaming 중인 UserID를 저장하는 배열 델리게이트 변수
   UPROPERTY(BlueprintReadOnly , ReplicatedUsing=OnRep_StreamingID)
   TArray<FString> ArrStreamingUserID;

   UFUNCTION()
   void OnRep_StreamingID();

   /*UPROPERTY(Replicated , BlueprintReadOnly , Replicated)
   TArray<FString> ConnectedPlayerNames;*/ //(미사용)

};