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
	DOREPLIFETIME(AK_GameState , MissionData); // MissionData를 Replicated 설정
	DOREPLIFETIME(AK_GameState , ConnectedPlayerNames); // ConnectedPlayerNames를 Replicated 설정
}

void AK_GameState::BeginPlay()
{
	Super::BeginPlay();

	auto gi = CastChecked<UK_GameInstance>(GetGameInstance());
	if ( gi )
	{
		SetMissionData(gi->MissionData);
		SetConnectedPlayerNames(gi->ConnectedPlayerNames);
	}
	
}

//캐릭터의 StreamingUI에 보유중인 UserID 배열 델리게이트를 전달
void AK_GameState::OnRep_StreamingID()
{
	LOG_S(Warning , TEXT("ArrStreamingUserID Num : %d") , ArrStreamingUserID.Num());

	// SharingSlot위젯에 방송중인 플레이어 리스트 업데이트
	auto Me = Cast<AMTVS_AirJet_FinalCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	if ( Me )
	{
		Me->StreamingUI->InitSlot(ArrStreamingUserID);
	}

}
//MissionData가 변경될때 클라이언트에 알리기 위한 RepNotify함수
void AK_GameState::OnRep_MissionData()
{
	// World에서 플레이어 컨트롤러를 가져와 StandbyWidget에 접근
	for ( FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It )
	{
		AK_PlayerController* PlayerController = Cast<AK_PlayerController>(It->Get());
		if ( PlayerController )
		{
			UK_StandbyWidget* StandbyWidget = Cast<UK_StandbyWidget>(PlayerController->StandbyUI);
			if ( StandbyWidget )
			{
				// StandbyWidget의 InitializeMissionData 호출로 UI 업데이트
				StandbyWidget->InitializeMissionData();
			}
		}
	}
}

//서버에서 MissionData설정하는 함수
void AK_GameState::SetMissionData(const FMissionDataRes& NewMissionData)
{

	MissionData = NewMissionData;
	OnRep_MissionData(); // 변경 후 즉시 RepNotify 호출
	
}

void AK_GameState::OnRep_ConnectedPlayerNames()
{
	// World에서 플레이어 컨트롤러를 가져와 StandbyWidget에 접근
	LOG_S(Warning, TEXT("Player Count : %d"), ConnectedPlayerNames.Num());

	for ( FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It )
	{
		AK_PlayerController* PlayerController = Cast<AK_PlayerController>(It->Get());
		if ( PlayerController )
		{
			UK_StandbyWidget* StandbyWidget = Cast<UK_StandbyWidget>(PlayerController->StandbyUI);
			if ( StandbyWidget )
			{
				// StandbyWidget의 SetPlayerList 호출로 UI 업데이트
				StandbyWidget->SetPlayerList();
			}
		}
	}
}

void AK_GameState::SetConnectedPlayerNames(const TArray<FString>& newNames)
{
	ConnectedPlayerNames = newNames;
	if( HasAuthority() )
		OnRep_ConnectedPlayerNames();
}
