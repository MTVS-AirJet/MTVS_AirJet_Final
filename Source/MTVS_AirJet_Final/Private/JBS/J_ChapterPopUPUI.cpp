// Fill out your copyright notice in the Description page of Project Settings.


#include "JBS/J_ChapterPopUPUI.h"
#include "Components/RichTextBlock.h"
#include "Components/SlateWrapperTypes.h"
#include "JBS/J_Utility.h"
#include "TimerManager.h"

void UJ_ChapterPopUPUI::SetPopupText(const EMissionProcess& mpIdx, const FString& normalStr)
{
    SetActive(true);
    
    // @@ 초상화 바뀔일이 있을까?

    // 텍스트 스타일대로 포맷해서 설정
    popupText->SetText(FText::FromString(FRichString(normalStr, textStyle).GetFormatString()));
}

void UJ_ChapterPopUPUI::SetActive(bool value)
{
    ESlateVisibility isVisible = value ? ESlateVisibility::Visible : ESlateVisibility::Hidden;

    this->SetVisibility(isVisible);

    // 딜리게이트 실행
    if(value)
    {
        activeDel.Broadcast();

        // FIXME 임시로 5초뒤 비활성화
        auto& tm = GetWorld()->GetTimerManager();
        tm.ClearTimer(tempDeactiveTimer);

        tm.SetTimer(tempDeactiveTimer, [this]() mutable
        {
            SetActive(false);
        }, 5.f, false);
    }
    else
        deactiveDel.Broadcast();
}