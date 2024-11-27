// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "L_PlayerNameWidget.generated.h"

/**
 * 
 */
UCLASS()
class MTVS_AIRJET_FINAL_API UL_PlayerNameWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* txtPlayerName;
};
