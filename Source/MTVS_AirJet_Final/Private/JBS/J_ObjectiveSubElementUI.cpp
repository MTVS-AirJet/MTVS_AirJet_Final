// Fill out your copyright notice in the Description page of Project Settings.


#include "JBS/J_ObjectiveSubElementUI.h"
#include "Components/RichTextBlock.h"
#include "Engine/DataTable.h"
#include "JBS/J_Utility.h"
#include "Styling/SlateTypes.h"

void UJ_ObjectiveSubElementUI::SetSubText(const FRichString& str)
{
    this->objSubText->SetText(FText::FromString(str.GetFormatString()));
}