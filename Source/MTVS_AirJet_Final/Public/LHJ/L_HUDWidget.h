// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "L_HUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class MTVS_AIRJET_FINAL_API UL_HUDWidget : public UUserWidget
{
	GENERATED_BODY()

private:
	virtual void NativeConstruct() override;

private:
	UPROPERTY(EditDefaultsOnly , meta=(BindWidget))
	class UProgressBar* prg_Height;
	UPROPERTY(EditDefaultsOnly , meta=(BindWidget))
	class UImage* img_Height;
	UPROPERTY(EditDefaultsOnly , meta=(BindWidget))
	class UTextBlock* txtHeight;
	UPROPERTY(EditDefaultsOnly , meta=(BindWidget))
	class UTextBlock* txtSpeed;

public:
	void UpdateHeightBar(float NewHeight);
	void UpdateHeightText(int32 NewHeight);
	void UpdateSpeedText(float NewSpeed);
};
