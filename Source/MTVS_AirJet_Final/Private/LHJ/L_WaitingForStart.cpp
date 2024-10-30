// Fill out your copyright notice in the Description page of Project Settings.


#include "LHJ/L_WaitingForStart.h"

#include "MTVS_AirJet_Final.h"
#include "Components/TextBlock.h"
#include "KHS/K_GameInstance.h"
#include "LHJ/L_Viper.h"

void UL_WaitingForStart::NativeConstruct()
{
	Super::NativeConstruct();

	gm = Cast<UK_GameInstance>(GetWorld()->GetGameInstance());
	MaxCnt = gm->ConnectedPlayerNames.Num();
	LOG_S(Warning, TEXT("Current Player Count : %d"), MaxCnt);
	FString txt = FString::Printf(TEXT("%d/%d") , CurrentCnt , MaxCnt);
	TxtReadyCnt->SetText(FText::FromString(txt));
}

void UL_WaitingForStart::SetMem(const int32& newMem)
{
	CurrentCnt = newMem;
	FString txt = FString::Printf(TEXT("%d/%d") , CurrentCnt , MaxCnt);
	TxtReadyCnt->SetText(FText::FromString(txt));

	if (MaxCnt == CurrentCnt || MaxCnt == 0)
	{
		if (auto viper = Cast<AL_Viper>(GetOwningPlayerPawn()))
			viper->ReadyAllMembers();
	}
}
