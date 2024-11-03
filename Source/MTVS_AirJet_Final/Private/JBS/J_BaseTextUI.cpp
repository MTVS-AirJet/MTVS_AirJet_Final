// Fill out your copyright notice in the Description page of Project Settings.


#include "JBS/J_BaseTextUI.h"
#include "Components/RichTextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/Widget.h"
#include "Internationalization/Text.h"
#include "Templates/Casts.h"
#include "Blueprint/WidgetTree.h"
#include "Templates/SubclassOf.h"

void UJ_BaseTextUI::SetTextUI(FTextUIData data)
{
    SetHeaderText(data.headerText);
    SetBodyText(data.bodyTextAry);
}

void UJ_BaseTextUI::SetHeaderText(const FString &str, URichTextBlock *headerText)
{
    headerText->SetText(FText::FromString(str));
}


void UJ_BaseTextUI::SetBodyText(TArray<FString> strAry, UVerticalBox *bodyTextVBox)
{
    // 본문 vbox 초기화
    bodyTextVBox->ClearChildren();

    // strAry 개수 만큼 richtext 생성
    for(auto str : strAry)
    {
        // 새 본문 요소 생성
        auto* newBodyElement = CreateBodyElement(UWidget::StaticClass(), str);

        // vbox에 추가
        bodyTextVBox->AddChildToVerticalBox(newBodyElement);
        auto* slot = CastChecked<UVerticalBoxSlot>(newBodyElement->Slot);

        // vbox 슬롯 설정(패딩 등)
        if(slot)
            SetBodyVBoxSlot(slot);
        // @@ 목표 ui 패딩 처리하는거 확인
    }
}

// 상속받아 사용시 이거는 재정의 해야함
UWidget *UJ_BaseTextUI::CreateBodyElement(TSubclassOf<UWidget> widgetClass, const FString &str)
{
    auto* newWidget = WidgetTree->ConstructWidget<UWidget>(widgetClass);
    auto* newRichText = CastChecked<URichTextBlock>(newWidget);
    
    newRichText->SetText(FText::FromString(str));

    return newRichText;
}