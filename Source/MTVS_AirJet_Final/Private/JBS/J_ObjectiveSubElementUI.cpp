// Fill out your copyright notice in the Description page of Project Settings.


#include "JBS/J_ObjectiveSubElementUI.h"
#include "Components/Image.h"
#include "Components/RichTextBlock.h"
#include "Engine/DataTable.h"
#include "JBS/J_Utility.h"
#include "Math/MathFwd.h"
#include "Styling/SlateTypes.h"

void UJ_ObjectiveSubElementUI::SetSubText(const FString& str)
{
    this->objSubText->SetText(FText::FromString(str));

    // FIXME 서브 목표 설명이면 이미지 사이즈 작게
    // if(str.styleType == ETextStyle::OBJDETAIL)
    // {
    //     objSubIcon->SetDesiredSizeOverride(FVector2D(subObjDetailiconSize,subObjDetailiconSize));
    // }
}