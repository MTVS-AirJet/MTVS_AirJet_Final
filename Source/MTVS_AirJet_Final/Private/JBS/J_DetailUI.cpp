// Fill out your copyright notice in the Description page of Project Settings.


#include "JBS/J_DetailUI.h"
#include "Blueprint/UserWidget.h"
#include "Components/RichTextBlock.h"
#include "Components/SlateWrapperTypes.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "JBS/J_ObjectiveSubElementUI.h"
#include "Layout/Margin.h"

void UJ_DetailUI::SetTextUI(FTextUIData data, bool isInit)
{
    SetHeaderText(data.headerText, detailHeaderText);
    SetBodyText(data.bodyTextAry, isInit, detailBodyElementVBox);

    // 최초 생성 시
    if(isInit)
    {
        // 애니메이션 실행은 bp에서
    }
}

void UJ_DetailUI::SetBodyTextValue(class UWidget *textWidget, const FString &str)
{
    auto* subEle = CastChecked<UJ_ObjectiveSubElementUI>(textWidget);

    subEle->objSubText->SetText(FText::FromString(str));
}

UWidget *UJ_DetailUI::CreateBodyElement(TSubclassOf<class UWidget> widgetClass, const FString &str)
{
    auto* newObjSubEle = CreateWidget<UJ_ObjectiveSubElementUI>(this, detailSubEleUIPrefab);
    // 값 설정
    SetBodyTextValue(newObjSubEle, str);

    return newObjSubEle;
}

void UJ_DetailUI::SetBodyVBoxSlot(class UVerticalBoxSlot *newSlot)
{
    newSlot->SetPadding(FMargin(0,0,0,20));
    newSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill); // 수평 정렬
    newSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Center); // 수직 정렬
    newSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));

}