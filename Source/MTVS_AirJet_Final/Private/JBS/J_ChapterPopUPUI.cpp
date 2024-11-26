// Fill out your copyright notice in the Description page of Project Settings.


#include "JBS/J_ChapterPopUPUI.h"
#include "Components/Image.h"
#include "Components/RichTextBlock.h"
#include "Components/SlateWrapperTypes.h"
#include "JBS/J_Utility.h"
#include "TimerManager.h"

void UJ_ChapterPopUPUI::SetPopupText(const EMissionProcess& mpIdx, const FString& normalStr, const float& newDelayTime)
{
    // @@ 초상화 바뀔일이 있을까?

    // 새 딜레이 시간 적용
    if(newDelayTime > 0.1f)
        deActiveDelay = newDelayTime;

    SetActive(true);

    // 텍스트 스타일대로 포맷해서 설정
    popupText->SetText(FText::FromString(FRichString(normalStr, textStyle).GetFormatString()));

    // 텍스트 이미지 변경
    if(textImgMap.Contains(mpIdx))
    {
        popupTextImg->SetBrush(textImgMap[mpIdx]);
    }
}

void UJ_ChapterPopUPUI::SetActive(bool value)
{
    ESlateVisibility isVisible = value ? ESlateVisibility::Visible : ESlateVisibility::Hidden;

    this->SetVisibility(isVisible);

    // 딜리게이트 실행
    if(value)
    {
        PlayActiveAnim();

        activeDel.Broadcast();

        // 시트에 정해진 시간 이후에 비활성화
        auto& tm = GetWorld()->GetTimerManager();
        tm.ClearTimer(deactiveTimer);

        tm.SetTimer(deactiveTimer, [this]() mutable
        {
            PlayDeactiveAnim();
        }, deActiveDelay, false);
    }
    else
        deactiveDel.Broadcast();
}