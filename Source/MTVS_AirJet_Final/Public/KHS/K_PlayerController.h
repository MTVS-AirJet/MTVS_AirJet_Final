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
	//------------------------------------------화면 공유 UI
   UPROPERTY(EditDefaultsOnly)
   TSubclassOf<class UUserWidget> StreamingUIFactory;
   UPROPERTY(BlueprintReadWrite)
   class UK_StreamingUI* StreamingUI;

};
