// Fill out your copyright notice in the Description page of Project Settings.


#include "KHS/K_GameState.h"
#include "KHS/K_StreamingUI.h"
#include <Net/UnrealNetwork.h>
#include <MTVS_AirJet_Final.h>
#include "MTVS_AirJet_FinalCharacter.h"

void AK_GameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{	
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AK_GameState , ArrStreamingUserID);
}

void AK_GameState::OnRep_StreamingID()
{	
	LOG_S(Warning , TEXT("ArrStreamingUserID Num : %d") , ArrStreamingUserID.Num());

	// 리스트에 방송중인 플레이어 리스트 뿌려주기
	auto Me = Cast<AMTVS_AirJet_FinalCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	if ( Me )
	{
		Me->StreamingUI->InitSlot(ArrStreamingUserID);
	}

}
