// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "JBS/J_BaseTextUI.h"
#include "J_ObjectiveTextUI.generated.h"

/**
 * 
 */
DECLARE_DELEGATE_OneParam(FUIAnimDel, class UJ_ObjectiveTextUI*);

UCLASS()
class MTVS_AIRJET_FINAL_API UJ_ObjectiveTextUI : public UJ_BaseTextUI
{
	GENERATED_BODY()
protected:
	// 캔바스 패널
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|UI", meta = (BindWidget))
	class USizeBox* objectiveUI;
	// 배경 이미지
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|UI", meta = (BindWidget))
	class UImage* objectiveUIBG;

	// 전술명령 헤더 텍스트
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|UI", meta = (BindWidget))
	class URichTextBlock* objHeaderText;
	
	// 본문 서브 조건 ui vbox
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|UI", meta = (BindWidget))
	class UVerticalBox* objBodyElementVBox;
		public:
	__declspec(property(get = GetBodyVBox, put = SetBodyVBox)) class UVerticalBox* OBJ_BODY_VBOX;
	class UVerticalBox* GetBodyVBox()
	{
		return objBodyElementVBox;
	}
	void SetBodyVBox(class UVerticalBox* value)
	{
		objBodyElementVBox = value;
	}
		protected:

	// 서브 조건 요소 UI 프리팹
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Classes")
	TSubclassOf<class UJ_ObjectiveSubElementUI> objSubEleUIPrefab;
public:
	// 시작 애니메이션 딜리게이트
	FUIAnimDel startAnimDel;

protected:
	// 본문 텍스트 박스 설정(패딩 등등)
    void SetBodyVBoxSlot(class UVerticalBoxSlot *newSlot) override;
    void SetBodyTextValue(class UWidget *textWidget, const FRichString &str) override;

public:
	// 명령 UI 설정
    void SetTextUI(FTextUIData data, bool isInit = false) override;

	// 서브 조건 요소 설정
    UWidget *CreateBodyElement(TSubclassOf<class UWidget> widgetClass, const FRichString &str) override;

	// ui 애니메이션
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void PlayStartAnim(const TArray<UWidget*>& allSub);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void PlayEndAnim(const TArray<UWidget*>& allSub);
	
};
