// Fill out your copyright notice in the Description page of Project Settings.


#include "LHJ/L_HUDWidget.h"

#include "MTVS_AirJet_Final.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"


void UL_HUDWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UL_HUDWidget::UpdateHeightBar(float NewHeight)
{
	float newPercent = NewHeight / 1500;
	prg_Height->SetPercent(newPercent);
	UpdateHeightText(NewHeight);

	if (NewHeight <= 1400)
	{
		img_Height->SetVisibility(ESlateVisibility::Visible);
		prg_Height->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		img_Height->SetVisibility(ESlateVisibility::Hidden);
		prg_Height->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UL_HUDWidget::UpdateHeightText(int32 NewHeight)
{
	FString h = FString::FormatAsNumber(NewHeight);
	txtHeight->SetText(FText::FromString(h));
}

void UL_HUDWidget::UpdateSpeedText(float NewSpeed)
{
	txtSpeed->SetText(FText::AsNumber(static_cast<int32>(NewSpeed)));
}
