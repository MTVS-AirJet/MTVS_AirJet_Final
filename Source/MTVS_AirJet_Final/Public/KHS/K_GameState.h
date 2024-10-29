// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include <JBS/J_Utility.h>
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
	virtual void BeginPlay() override;

public:
   //=================화면공유 관련 정보 저장
   //Streaming 중인 UserID를 저장하는 배열 델리게이트 변수
   UPROPERTY(BlueprintReadOnly , ReplicatedUsing=OnRep_StreamingID)
   TArray<FString> ArrStreamingUserID;

   UFUNCTION()
   virtual void OnRep_StreamingID();

   /*UPROPERTY(Replicated , BlueprintReadOnly , Replicated)
   TArray<FString> ConnectedPlayerNames;*/ //(미사용)

public:
	//RepNotify로 관리할 미션데이터
	UPROPERTY(ReplicatedUsing = OnRep_MissionData)
	FMissionDataRes MissionData;
	
	//MissionData가 변경될때 클라이언트에 알리기 위한 RepNotify함수
	UFUNCTION()
	void OnRep_MissionData();

	//서버에서 MissionData설정하는 함수
	void SetMissionData(const FMissionDataRes& NewMissionData);

public:
	//RepNotify로 관리할 미션데이터
	UPROPERTY(ReplicatedUsing = OnRep_ConnectedPlayerNames)
	TArray<FString> ConnectedPlayerNames;

	UFUNCTION()
	void OnRep_ConnectedPlayerNames();

	void SetConnectedPlayerNames(const TArray<FString>& newNames);


};
