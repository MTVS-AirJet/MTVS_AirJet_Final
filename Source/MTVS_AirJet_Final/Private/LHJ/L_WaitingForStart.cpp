// Fill out your copyright notice in the Description page of Project Settings.


#include "LHJ/L_WaitingForStart.h"

#include "MTVS_AirJet_Final.h"
#include "Components/TextBlock.h"
#include "KHS/K_GameInstance.h"
#include "KHS/K_GameState.h"
#include "LHJ/L_Viper.h"

void UL_WaitingForStart::NativeConstruct()
{
	Super::NativeConstruct();

	gi = Cast<UK_GameInstance>(GetGameInstance());
	gs = Cast<AK_GameState>(GetWorld()->GetGameState());	
	// LOG_S(Warning, TEXT("Current Player Count : %d"), MaxCnt);
	FString txt = FString::Printf(TEXT("%d/%d") , CurrentCnt , MaxCnt);
	TxtReadyCnt->SetText(FText::FromString(txt));
}

void UL_WaitingForStart::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	// Super::NativeTick(MyGeometry , InDeltaTime);
	// MaxCnt = gs->ConnectedPlayerNames.Num();
	// //CurrentCnt = gi->ReadyMemeberCnt;
	// FString txt = FString::Printf(TEXT("%d/%d") , CurrentCnt , MaxCnt);
	// TxtReadyCnt->SetText(FText::FromString(txt));
	//
	// if (MaxCnt == CurrentCnt || MaxCnt == 0)
	// {
	// 	// if (auto viper = Cast<AL_Viper>(GetOwningPlayerPawn()))
	// 	// 	viper->ReadyAllMembers();
	// }
}

void UL_WaitingForStart::SetMem(const int32& newMem)
{
	MaxCnt = gs->ConnectedPlayerNames.Num();
	CurrentCnt = newMem;
	//CurrentCnt = gi->ReadyMemeberCnt;
	// LOG_SCREEN("Current Mem: %d", CurrentCnt);;
	FString txt = FString::Printf(TEXT("%d/%d") , CurrentCnt , MaxCnt);
	TxtReadyCnt->SetText(FText::FromString(txt));

	if (MaxCnt == CurrentCnt || MaxCnt == 0)
	{
		// if (auto viper = Cast<AL_Viper>(GetOwningPlayerPawn()))
		// 	viper->ReadyAllMembers();
	}
}
