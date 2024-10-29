// Fill out your copyright notice in the Description page of Project Settings.


#include "KHS/K_PlayerController.h"
#include "KHS/K_StandbyWidget.h"
#include "KHS/K_GameInstance.h"
#include "LHJ/L_Viper.h"
#include "Kismet/GameplayStatics.h"
#include <MTVS_AirJet_Final.h>
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputSubsystems.h"

void AK_PlayerController::BeginPlay()
{
	Super::BeginPlay();


}

void AK_PlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	CurrentMapName = UGameplayStatics::GetCurrentLevelName(GetWorld());
	//로그찍기
	LOG_S(Warning , TEXT("KPlayerController OnPossess!!!!!"));
	LOG_S(Warning , TEXT("PlayerController %s possessed pawn %s") , *GetName() , *InPawn->GetName());
	LOG_S(Warning , TEXT("Current Map Name : %s"), *CurrentMapName);
	LOG_S(Warning , TEXT("Current Player Controller Count : %d"), GetWorld()->GetNumPlayerControllers());
	TArray<AActor*> outActor;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AL_Viper::StaticClass(), outActor);
	LOG_S(Warning , TEXT("Current Viper Count : %d") , outActor.Num());


	// 서버에서 Pawn 타입을 확인하고 적절한 UI 설정 함수를 호출
	CurrentMapName = UGameplayStatics::GetCurrentLevelName(GetWorld());
	if ( CurrentMapName == FString::Printf(TEXT("CesiumTest")) )
	{
		//CRPC로 UI세팅하고 IMC맵핑
		CRPC_SetIMCnCreateStandbyUI();
		LOG_S(Warning , TEXT("KPlayerController Call Server_NotifyPawnPossessed!!!!!"));
	}
}

//클라이언트 UI생성 및 IMC맵핑 RPC함수
void AK_PlayerController::CRPC_SetIMCnCreateStandbyUI_Implementation()
{
	if ( IsLocalController() )
	{
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
		if ( Subsystem )
		{

			if ( nullptr == IMC_Viper )
			{
				UE_LOG(LogTemp , Log , TEXT("nullptr == IMC_Viper"));
			}
			else {
				Subsystem->ClearAllMappings();
				Subsystem->AddMappingContext(IMC_Viper , 0);
				UE_LOG(LogTemp , Log , TEXT("EnhancedInputMappingContext Mapping Complete"));
			}
		}
		else
		{
			UE_LOG(LogTemp , Error , TEXT("Subsystem == null"));
		}


		StandbyUI = CreateWidget<UK_StandbyWidget>(this , StandbyUIFactory);
		CurrentMapName = UGameplayStatics::GetCurrentLevelName(GetWorld());
		if ( StandbyUI && CurrentMapName == FString::Printf(TEXT("CesiumTest")) )
		{
			StandbyUI->SetUI();
			StandbyUI->InitializeMissionData(); // 서버에서 설정한 데이터와 동기화


			if ( UK_GameInstance* GI = Cast<UK_GameInstance>(GetGameInstance()) )
			{
				if ( StandbyUI )
				{
					StandbyUI->ClientUpdatePlayerList(GI->ConnectedPlayerNames);  // StandbyUI에서 PlayerList 동기화
				}
			}
		}
	}
	else
		LOG_S(Warning , TEXT("Is Not Local Controller"));
}

//void AK_PlayerController::ClientRPC_UpdatePlayerList_Implementation(const TArray<FString>& playerNames)
//{
//	
//}

////클라이언트가 UI업로드 후 서버에 업데이트 수신RPC 함수
//void AK_PlayerController::ServerRPC_RequestPlayerListUpdate_Implementation()
//{
//	if ( UK_GameInstance* GI = Cast<UK_GameInstance>(GetGameInstance()) )
//	{
//		GI->SendPlayerListToClient(this);
//	}
//}


void AK_PlayerController::TravelToLobbyLevel()
{
	// 로비 맵으로 클라이언트를 이동
	ClientTravel("/Game/Maps/KHS/K_LobbyMap" , ETravelType::TRAVEL_Absolute);

}
