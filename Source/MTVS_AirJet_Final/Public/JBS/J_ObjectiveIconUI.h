// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "J_ObjectiveIconUI.generated.h"

/**
 * 
 */
UCLASS()
class MTVS_AIRJET_FINAL_API UJ_ObjectiveIconUI : public UUserWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|UI", meta = (BindWidget))
	class UImage* objIconImage;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|UI", meta = (BindWidget))
	class UTextBlock* objDisText;

public:

protected:


public:
	UFUNCTION(BlueprintCallable)
	void SetObjDisText(float distance);
};
