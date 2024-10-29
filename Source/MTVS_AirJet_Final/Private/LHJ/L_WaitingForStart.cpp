// Fill out your copyright notice in the Description page of Project Settings.


#include "LHJ/L_WaitingForStart.h"

#include "Components/TextBlock.h"
#include "KHS/K_GameInstance.h"
#include "LHJ/L_Viper.h"

void UL_WaitingForStart::NativeConstruct()
{
	Super::NativeConstruct();

	viper = Cast<AL_Viper>(GetOwningPlayer());
	gm = Cast<UK_GameInstance>(GetWorld()->GetGameInstance());
	MaxCnt = gm->ConnectedPlayerNames.Num();
	FString txt=FString::Printf(TEXT("%d/%d"), CurrentCnt , MaxCnt);
	TxtReadyCnt->SetText(FText::FromString(txt));
}

void UL_WaitingForStart::NativeTick(const FGeometry& MyGeometry , float InDeltaTime)
{
	Super::NativeTick(MyGeometry , InDeltaTime);

	// MaxCnt = gm->ConnectedPlayerNames.Num();
	// FString txt=FString::Printf(TEXT("%d/%d"), CurrentCnt , MaxCnt);
	// TxtReadyCnt->SetText(FText::FromString(txt));
}
