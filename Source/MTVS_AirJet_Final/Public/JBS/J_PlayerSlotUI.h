// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "J_PlayerSlotUI.generated.h"

/**
 * 
 */
UCLASS()
class MTVS_AIRJET_FINAL_API UJ_PlayerSlotUI : public UUserWidget
{
	GENERATED_BODY()
protected:
	// 플레이어 선택 버튼
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|UI", meta = (BindWidget))
	class UButton* playerSelectBtn;

	// 플레이어 id 텍스트
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|UI", meta = (BindWidget))
	class UTextBlock* playerIdText;

public:

protected:


public:
	// ui text, 버튼 딜리게이트 init
};
