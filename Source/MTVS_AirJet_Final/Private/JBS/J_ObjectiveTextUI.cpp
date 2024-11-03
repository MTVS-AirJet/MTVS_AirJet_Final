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

void UJ_ObjectiveTextUI::SetTextUI(FTextUIData data)
{
    SetHeaderText(data.headerText, objHeaderText);
    SetBodyText(data.bodyTextAry, objBodyElementVBox);
}

UWidget *UJ_ObjectiveTextUI::CreateBodyElement(TSubclassOf<class UWidget> widgetClass, const FString &str)
{
    auto* newObjSubEle = CreateWidget<UJ_ObjectiveSubElementUI>(GetWorld(), objSubEleUIPrefab);
    // str 값 설정
    newObjSubEle->objSubText->SetText(FText::FromString(str));

    return newObjSubEle;
}

void UJ_ObjectiveTextUI::UpdateObjUIAnimValue(float canvasX, float bgPaddingBottom, float subEleScaleY)
{
    // 좌측으로 이동
    auto curSizeBoxTR = objectiveUI->GetRenderTransform();
    curSizeBoxTR.Translation.X = canvasX;
    
    objectiveUI->SetRenderTransform(curSizeBoxTR);

    // bg 늘어나기
    auto* bgSlot = CastChecked<UOverlaySlot>(objectiveUIBG->Slot);
    auto curPadding = bgSlot->GetPadding();
    curPadding.Bottom = bgPaddingBottom;

    bgSlot->SetPadding(curPadding);

    // 서브 조건 나타나기
    // 모든 서브 조건 가져오기

    // FIXME 왜 안되는겨
    // auto allChildren =  objBodyElementVBox->GetAllChildren();
    // for(auto* subEle : allChildren)
    // {
    //     // Y 값 적용
    //     subEle->SetRenderScale(FVector2D(1, subEleScaleY));
    // }
    // @@ 대체
    objBodyElementVBox->SetRenderScale(FVector2D(1, subEleScaleY));

}

void UJ_ObjectiveTextUI::SetBodyVBoxSlot(class UVerticalBoxSlot *newSlot)
{
    newSlot->SetPadding(FMargin(0,0,0,25));
    newSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill); // 수평 정렬
    newSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Center); // 수직 정렬
    newSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));

}