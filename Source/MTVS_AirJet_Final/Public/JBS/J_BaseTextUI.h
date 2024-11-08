// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "JBS/J_Utility.h"
#include "J_BaseTextUI.generated.h"

/**
 * 
 */
UCLASS()
class MTVS_AIRJET_FINAL_API UJ_BaseTextUI : public UUserWidget
{
	GENERATED_BODY()
protected:
	// 헤더 텍스트
	// UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|UI", meta = (BindWidget))
	// class URichTextBlock* headerText;
	// // 본문 텍스트 용 리스트 VBox
	// UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|UI", meta = (BindWidget))
	// class UVerticalBox* bodyTextVBox;

public:

protected:
	// 본문 텍스트 박스 설정(패딩 등등)
	virtual void SetBodyVBoxSlot(class UVerticalBoxSlot* newSlot) {}
	// 본문 값 설정
    virtual void SetBodyTextValue(class UWidget *textWidget, const FRichString &str);

public:
	// 새 텍스트 설정 | 상속시 재정의 필요
    UFUNCTION(BlueprintCallable)
    virtual void SetTextUI(FTextUIData data, bool isInit = false);


	// 헤더 텍스트 설정 
    virtual void SetHeaderText(const FRichString &str, class URichTextBlock *headerText = nullptr);
    // 본문 설정 
    virtual void SetBodyText(const TArray<FRichString>& strAry, bool isInit = false, class UVerticalBox *bodyTextVBox = nullptr);
	// 본문 요소 생성 | 상속시 재정의 필요
    virtual UWidget *CreateBodyElement(TSubclassOf<class UWidget> widgetClass, const FRichString &str);
	
};