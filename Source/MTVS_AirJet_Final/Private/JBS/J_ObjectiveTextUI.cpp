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
#include "JBS/J_Utility.h"
#include "Layout/Margin.h"
#include "Math/MathFwd.h"
#include "Math/UnrealMathUtility.h"
#include "Slate/WidgetTransform.h"
#include "Components/Image.h"
#include "Components/OverlaySlot.h"
#include "Templates/Casts.h"
#include "Components/Overlay.h"

void UJ_ObjectiveTextUI::SetTextUI(FTextUIData data, bool isInit)
{
    SetHeaderText(data.headerText, objHeaderText);
    // 복잡 구조와 단순 구조에 따라 다르게 처리
    if(data.bodyTextAry.Num() > 0)
        SetBodyText(data.bodyTextAry, isInit, objBodyElementVBox);
    else
        SetBodyText(data.bodyObjAry, isInit, objBodyElementVBox);

    // 최초 생성 시
    if(isInit)
    {
        // ui 설정 완료 딜리게이트
        startAnimDel.ExecuteIfBound(this);
        startAnimDel.Unbind();
    }
}

void UJ_ObjectiveTextUI::SetTextUI(FDefaultTextUIData data, bool isInit)
{
    SetHeaderText(data.headerText, objHeaderText);
    // 복잡 구조와 단순 구조에 따라 다르게 처리
    if(data.bodyTextAry.Num() > 0)
        SetBodyText(data.bodyTextAry, isInit, objBodyElementVBox);

    // 최초 생성 시
    if(isInit)
    {
        // ui 설정 완료 딜리게이트
        startAnimDel.ExecuteIfBound(this);
        startAnimDel.Unbind();
    }
}

void UJ_ObjectiveTextUI::SetBodyText(const TArray<FString> &strAry, bool isInit, class UVerticalBox *bodyTextVBox)
{
    Super::SetBodyText(strAry, isInit, bodyTextVBox);

}

void UJ_ObjectiveTextUI::SetBodyText(const TArray<FDefaultTextUIData> &objAry, bool isInit, class UVerticalBox *bodyTextVBox)
{
    // 최초 생성
    if(isInit)
    {
        // 본문 vbox 초기화
        clearChildDel.ExecuteIfBound();
        bodyTextVBox->ClearChildren();
        // objAry 개수 만큼 objUI 생성
        for(auto objData : objAry)
        {
            // 새 본문 요소 생성
            auto* newBodyElement = CreateBodyElement(UWidget::StaticClass(), objData);

            // vbox에 추가
            bodyTextVBox->AddChildToVerticalBox(newBodyElement);
            auto* slot = CastChecked<UVerticalBoxSlot>(newBodyElement->Slot);

            // vbox 슬롯 설정(패딩 등)
            if(slot)
                SetBodyVBoxSlot(slot);

            // @@
            newBodyElement->SetVisibility(ESlateVisibility::Hidden);
            auto size = slot->GetSize();
            size.SizeRule = ESlateSizeRule::Fill;
            size.Value = 0;
            slot->SetSize(size);
        }

        // @@ vbox의 자식 n 개 까지만 보여주기
        int view = FMath::Min3(3, objAry.Num(), bodyTextVBox->GetChildrenCount());
        int cnt = 0;
        for(int i = 0; i < view; i++)
        {
            auto* child = bodyTextVBox->GetChildAt(i);
            child->SetVisibility(ESlateVisibility::Visible);
            auto* slot = Cast<UVerticalBoxSlot>(child->Slot);
            auto size = slot->GetSize();
            size.SizeRule = ESlateSizeRule::Fill;
            size.Value = 1;
            slot->SetSize(size);
        }
    }
    else {
        for(int i = 0; i < FMath::Min(objAry.Num(), bodyTextVBox->GetChildrenCount()) ; i++)
        {
            // 본문 값 설정
            auto& objData = objAry[i];
            auto* subUI = bodyTextVBox->GetChildAt(i);
            if(!subUI) return;

            auto* subObj = Cast<UJ_ObjectiveTextUI>(subUI);
            if(!subObj) return;

            subObj->SetTextUI(objData);
        }
    }

    
     
}

void UJ_ObjectiveTextUI::SetBodyTextValue(class UWidget *textWidget, const FString &str)
{
    auto* subEle = Cast<UJ_ObjectiveSubElementUI>(textWidget);
    if(!subEle) return;

    subEle->SetSubText(str);
}

UWidget *UJ_ObjectiveTextUI::CreateBodyElement(TSubclassOf<class UWidget> widgetClass, const FString &str)
{
    auto* newObjSubEle = CreateWidget<UJ_ObjectiveSubElementUI>(this, objSubEleUIPrefab);
    // 값 설정
    SetBodyTextValue(newObjSubEle, str);

    return newObjSubEle;
}

UWidget *UJ_ObjectiveTextUI::CreateBodyElement(TSubclassOf<class UWidget> widgetClass, const FDefaultTextUIData &data)
{
    auto* newSubObj = CreateWidget<UJ_ObjectiveTextUI>(this, objSubObjUIPrefab);
    // 값 설정
    newSubObj->SetTextUI(data);

    return newSubObj;
}

void UJ_ObjectiveTextUI::SetBodyVBoxSlot(class UVerticalBoxSlot *newSlot)
{
    // @@ 시동 절차 이상해져서 뺏음
    newSlot->SetPadding(FMargin(20,0,0,5));
    newSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill); // 수평 정렬
    newSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Center); // 수직 정렬
    newSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));

}

void UJ_ObjectiveTextUI::SetAllChildrenSize(float value)
{
    const auto& children = this->OBJ_BODY_VBOX->GetAllChildren();
    for(auto* child : children)
    {
        auto* subEle = Cast<UJ_ObjectiveSubElementUI>(child);
        if(!subEle) continue;

        subEle->SetSpacerSize(value);
    }
}