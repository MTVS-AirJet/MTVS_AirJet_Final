// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "JBS/J_BaseTextUI.h"
#include "J_ObjectiveTextUI.generated.h"

/**
 * 
 */
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

	// 서브 조건 요소 UI 프리팹
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Classes")
	TSubclassOf<class UJ_ObjectiveSubElementUI> objSubEleUIPrefab;

public:

protected:
	// 본문 텍스트 박스 설정(패딩 등등)
    void SetBodyVBoxSlot(class UVerticalBoxSlot *newSlot) override;

public:
	// 명령 UI 설정
    void SetTextUI(FTextUIData data) override;

	// 서브 조건 요소 설정
    UWidget *CreateBodyElement(TSubclassOf<class UWidget> widgetClass, const FString &str) override;

	

	// 전술명령 UI 요소 값 적용 | 애니메이션에 사용
    void UpdateObjUIAnimValue(float canvasX, float bgPaddingBottom, float subEleScaleY);
};
