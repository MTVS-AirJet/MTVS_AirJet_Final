// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KHS/K_WidgetBase.h"
#include "K_LobbyWidget.generated.h"

/**
 * 
 */
UCLASS()
class MTVS_AIRJET_FINAL_API UK_LobbyWidget : public UK_WidgetBase
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct() override;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* txt_PlayerName; // PlayerName

	void SetTextLobbyName(FString PlayerName);

	
};
