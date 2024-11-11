// Fill out your copyright notice in the Description page of Project Settings.


#include "KHS/K_LobbyWidget.h"
#include "Components/TextBlock.h"
#include "KHS/K_GameInstance.h"

void UK_LobbyWidget::NativeConstruct()
{
	Super::NativeConstruct();

	//레벨의 GameInstance를 가져온다.
	auto GameInstance = Cast<UK_GameInstance>(GetGameInstance());
	if(GameInstance)
	{
		SetTextLobbyName(GameInstance->GetUserId());
	}
}

void UK_LobbyWidget::SetTextLobbyName(FString PlayerName)
{
	txt_PlayerName->SetText(FText::FromString(PlayerName));
}
