// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "JBS/J_Utility.h"
#include "J_ObjectiveSubElementUI.generated.h"

/**
 * 
 */

UCLASS()
class MTVS_AIRJET_FINAL_API UJ_ObjectiveSubElementUI : public UUserWidget
{
	GENERATED_BODY()
protected:

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|UI", meta = (BindWidget))
	class UImage* objSubIcon;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|UI", meta = (BindWidget))
	class URichTextBlock* objSubText;

protected:
	

public:
	// ui 애니메이션
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void PlayStartAnim();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void PlayEndAnim();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void PlaySubEndAnim();

	// 리치 텍스트 설정
	void SetSubText(const FRichString& str);
};
