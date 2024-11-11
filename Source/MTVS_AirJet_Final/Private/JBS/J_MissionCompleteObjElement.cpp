// Fill out your copyright notice in the Description page of Project Settings.


#include "JBS/J_MissionCompleteObjElement.h"
#include "Components/RichTextBlock.h"
#include "Components/TextBlock.h"
#include "JBS/J_Utility.h"

void UJ_MissionCompleteObjElement::SetObjText(const FString &headerText, const FString &bodyText)
{
    objNameText->SetText(FText::FromString(headerText));
    objResultText->SetText(FText::FromString(FRichString(bodyText, ETextStyle::RESULTDEFAULT).GetFormatString()));
}