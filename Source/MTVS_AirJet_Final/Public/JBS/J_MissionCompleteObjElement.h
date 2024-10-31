// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "J_MissionCompleteObjElement.generated.h"

/**
 * 
 */
UCLASS()
class MTVS_AIRJET_FINAL_API UJ_MissionCompleteObjElement : public UUserWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|UI", meta = (BindWidget))
	class UTextBlock* objNameText;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|UI", meta = (BindWidget))
	class UTextBlock* objResultText;

public:

protected:


public:	
	// 헤더와 값 FString 설정
    void SetObjText(const FString &headerText, const FString &bodyText);
};
