// Fill out your copyright notice in the Description page of Project Settings.


#include "JBS/J_ObjectiveTextUI.h"
#include "Blueprint/UserWidget.h"
#include "Components/RichTextBlock.h"
#include "Components/SizeBox.h"
#include "Components/SlateWrapperTypes.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/WidgetComponent.h"
#include "JBS/J_ObjectiveSubElementUI.h"
#include "Layout/Margin.h"
#include "Math/MathFwd.h"
#include "Slate/WidgetTransform.h"
#include "Components/Image.h"
#include "Components/OverlaySlot.h"
#include "Templates/Casts.h"
#include "Components/Overlay.h"

void UJ_ObjectiveTextUI::SetTextUI(FTextUIData data, bool isInit)
{
    SetHeaderText(data.headerText, objHeaderText);
    SetBodyText(data.bodyTextAry, isInit, objBodyElementVBox);

    // 최초 생성 시
    if(isInit)
    {
        // ui 설정 완료 딜리게이트
        startAnimDel.ExecuteIfBound(this);
        startAnimDel.Unbind();
    }
}

void UJ_ObjectiveTextUI::SetBodyTextValue(class UWidget *textWidget, const FString &str)
{
    auto* subEle = CastChecked<UJ_ObjectiveSubElementUI>(textWidget);

    subEle->objSubText->SetText(FText::FromString(str));
}

UWidget *UJ_ObjectiveTextUI::CreateBodyElement(TSubclassOf<class UWidget> widgetClass, const FString &str)
{
    auto* newObjSubEle = CreateWidget<UJ_ObjectiveSubElementUI>(this, objSubEleUIPrefab);
    // 값 설정
    SetBodyTextValue(newObjSubEle, str);

    return newObjSubEle;
}

void UJ_ObjectiveTextUI::SetBodyVBoxSlot(class UVerticalBoxSlot *newSlot)
{
    newSlot->SetPadding(FMargin(0,0,0,20));
    newSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill); // 수평 정렬
    newSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Center); // 수직 정렬
    newSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));

}

// XXX
// void UJ_ObjectiveTextUI::UpdateObjUIAnimValue(float canvasX, float bgPaddingBottom, float subEleScaleY)
// {
//     // 좌측으로 이동
//     auto curSizeBoxTR = objectiveUI->GetRenderTransform();
//     curSizeBoxTR.Translation.X = canvasX;
    
//     objectiveUI->SetRenderTransform(curSizeBoxTR);

//     // bg 늘어나기
//     auto* bgSlot = CastChecked<UOverlaySlot>(objectiveUIBG->Slot);
//     auto curPadding = bgSlot->GetPadding();
//     curPadding.Bottom = bgPaddingBottom;

//     bgSlot->SetPadding(curPadding);

//     // objBodyElementVBox->SetRenderScale(FVector2D(1, subEleScaleY));

// }

