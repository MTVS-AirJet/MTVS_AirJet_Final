// Fill out your copyright notice in the Description page of Project Settings.


#include "KHS/K_GameState.h"
#include "KHS/K_StreamingUI.h"
#include "KHS/K_PlayerController.h"
#include "KHS/K_StandbyWidget.h"
#include "KHS/K_GameInstance.h"
#include <Net/UnrealNetwork.h>
#include <MTVS_AirJet_Final.h>
#include "MTVS_AirJet_FinalCharacter.h"

//델리게이트 변수 연동
void AK_GameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AK_GameState , ArrStreamingUserID);
	DOREPLIFETIME(AK_GameState, ConnectedPlayerNames);
}

void AK_GameState::BeginPlay()
{
	Super::BeginPlay();
}

//캐릭터의 StreamingUI에 보유중인 UserID 배열 델리게이트를 전달
void AK_GameState::OnRep_StreamingID()
{
	// LOG_S(Warning , TEXT("ArrStreamingUserID Num : %d") , ArrStreamingUserID.Num());

	// SharingSlot위젯에 방송중인 플레이어 리스트 업데이트
	auto Me = Cast<AMTVS_AirJet_FinalCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	if (Me)
	{
		Me->StreamingUI->InitSlot(ArrStreamingUserID);
	}
}

void AK_GameState::SetConnectedPlayerNames(const FString& newName)
{
	if ( !ConnectedPlayerNames.Contains(newName) )
	{
		ConnectedPlayerNames.Add(newName);
	}
	
	// LOG_S(Warning , TEXT("Player Count : %d") , ConnectedPlayerNames.Num());
	
}

TArray<FString> AK_GameState::GetConnectedPlayernames()
{
	return ConnectedPlayerNames;
}
