// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "K_PlayerController.generated.h"

/**
 * 
 */
UCLASS()
class MTVS_AIRJET_FINAL_API AK_PlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	void BeginPlay() override;

	void OnPossess(APawn* InPawn) override; // 캐릭터가 Possess될 때 호출되는 함수

	//화면 공유 UI======================================
   UPROPERTY(EditDefaultsOnly)
   TSubclassOf<class UUserWidget> StreamingUIFactory;
   UPROPERTY(BlueprintReadWrite)
   class UK_StreamingUI* StreamingUI;

   //인게임 StandbyUI===================================
   UPROPERTY(EditDefaultsOnly)
   TSubclassOf<class UUserWidget> StandbyUIFactory;
   UPROPERTY(BlueprintReadWrite)
   class UK_StandbyWidget* StandbyUI;

   FString CurrentMapName; //현재 맵네임

    UPROPERTY(EditDefaultsOnly , Category="Inputs")
	class UInputMappingContext* IMC_Viper;

   //클라이언트 UI생성 및 imc 맵핑 RPC함수
   UFUNCTION(Client, Reliable)
   void CRPC_SetIMCnCreateStandbyUI();

   //클라이언트 PlayerList Update RPC함수
   //UFUNCTION(Client, Reliable)
   //void ClientRPC_UpdatePlayerList(const TArray<FString>& playerNames);

   //클라이언트가 UI업로드 후 서버에 업데이트 수신RPC 함수
   /*UFUNCTION(Server, Reliable)
   void ServerRPC_RequestPlayerListUpdate();*/

    // 클라이언트를 로비 레벨로 트래블시키는 함수
    UFUNCTION(BlueprintCallable)
    virtual void TravelToLobbyLevel();

};
