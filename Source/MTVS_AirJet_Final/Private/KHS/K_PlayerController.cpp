// Fill out your copyright notice in the Description page of Project Settings.


#include "KHS/K_PlayerController.h"
#include "KHS/K_StandbyWidget.h"
#include "KHS/K_GameInstance.h"
#include "KHS/K_CommonWidget.h"
#include "KHS/K_WidgetBase.h"
#include "KHS/K_GameState.h"
#include "KHS/K_ServerWidget.h"
#include "LHJ/L_Viper.h"
#include <MTVS_AirJet_Final.h>

#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputSubsystems.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"

void AK_PlayerController::BeginPlay()
{
	Super::BeginPlay();

	// 인게임 Common UI 세팅
	if (CommonWidgetFactory)
	{
		CommonWidget = CreateWidget<UK_CommonWidget>(this , CommonWidgetFactory);
		if (CommonWidget)
		{
			CommonWidget->SetInterface(Cast<IK_SessionInterface>(GetGameInstance()));
			CommonWidget->AddToViewport(1);
			CommonWidget->SetVisibility(ESlateVisibility::Hidden);
			bIsCommonWidgetVisible = false; //평소엔 안보이게 처리
		}
	}

	//IMC Common 맵핑
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
		GetLocalPlayer());
	if (Subsystem)
	{
		Subsystem->AddMappingContext(IMC_Common , 0); //Zorder를 1번으로 설정.
	}

	//마우스커서는 평소엔 안보이게 처리
	bIsMouseCursorShow = false;
}

void AK_PlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	CurrentMapName = UGameplayStatics::GetCurrentLevelName(GetWorld());
	//로그찍기
	LOG_S(Warning , TEXT("KPlayerController OnPossess!!!!!"));
	LOG_S(Warning , TEXT("PlayerController %s possessed pawn %s") , *GetName() , *InPawn->GetName());
	LOG_S(Warning , TEXT("Current Map Name : %s") , *CurrentMapName);
	LOG_S(Warning , TEXT("Current Player Controller Count : %d") , GetWorld()->GetNumPlayerControllers());
	TArray<AActor*> outActor;
	UGameplayStatics::GetAllActorsOfClass(GetWorld() , AL_Viper::StaticClass() , outActor);
	LOG_S(Warning , TEXT("Current Viper Count : %d") , outActor.Num());


	// 서버에서 Pawn 타입을 확인하고 적절한 UI 설정 함수를 호출
	CurrentMapName = UGameplayStatics::GetCurrentLevelName(GetWorld());
	if (CurrentMapName == FString::Printf(TEXT("CesiumTest")))
	{
		//CRPC로 UI세팅하고 IMC맵핑
		CRPC_SetIMCnCreateStandbyUI();
		LOG_S(Warning , TEXT("KPlayerzController Call Server_NotifyPawnPossessed!!!!!"));
	}
}

void AK_PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent);

	if (EnhancedInput)
	{
		EnhancedInput->BindAction(IA_ToggleCommonWidget , ETriggerEvent::Started , this ,
		                          &AK_PlayerController::ToggleCommonWidget);
		EnhancedInput->BindAction(IA_ToggleMouseCursor , ETriggerEvent::Started , this ,
		                          &AK_PlayerController::ToggleMouseCursor);
		EnhancedInput->BindAction(IA_ThrottleButton7, ETriggerEvent::Triggered, this, &AK_PlayerController::ToggleCommonWidget);
		EnhancedInput->BindAction(IA_RemoveUI, ETriggerEvent::Triggered, this, &AK_PlayerController::RemoveStandbyWidget);
	}
}

// 전체 클라에 게임시작 선언 델리게이트 바인딩
void AK_PlayerController::SRPC_StartGame_Implementation()
{
	//Host PC가 아니면 Return
	auto pc = Cast<AK_PlayerController>(UGameplayStatics::GetPlayerController(GetWorld() , 0));
	if(pc != this)
		return;
	
	LOG_S(Warning , TEXT("Start Game SRPC is Called"));
	//월드에 존재하는 PlayerController배열 받기
	KGameState = CastChecked<AK_GameState>(UGameplayStatics::GetGameState(GetWorld()));
	TArray<AK_PlayerController*> allPC;
	Algo::Transform(KGameState->PlayerArray, allPC, [](TObjectPtr<APlayerState> PS)
	{
		check(PS);
		auto* tempPC = CastChecked<AK_PlayerController>(PS->GetPlayerController());
		check(tempPC);
		return tempPC; //tempPC를 차곡차곡 allPC에 넣고
	});

	//PC배열에 전체 CRPC작동 명령
	for(auto localPC : allPC)
	{
		localPC->CRPC_StartGame();
	}
	//Mission용 Del 실행
	StartGameDel_Mission.Broadcast();
}

bool AK_PlayerController::SRPC_StartGame_Validate()
{
	return true;
}

// 전체 클라에 게임시작 전달 Client RPC
void AK_PlayerController::CRPC_StartGame_Implementation()
{
	if(StandbyUI)
	{
		StandbyUI->RemoveUI();
	}
	StartGameDel_Viper.Broadcast();
}




//Common Widget 토글 함수
void AK_PlayerController::ToggleCommonWidget(const FInputActionValue& value)
{
	if (!IsLocalPlayerController()) return;

	if (nullptr == CommonWidget)
	{
		LOG_S(Warning , TEXT("CommonWidget is not Valid"));
		return;
	}
	//플래그에 따라 UI상태 제어
	if (bIsCommonWidgetVisible)
	{
		// if (CommonWidget->IsInViewport())
		// 	CommonWidget->RemoveFromParent();
		CommonWidget->SetVisibility(ESlateVisibility::Hidden);
		FInputModeGameOnly InputGameOnly;
		SetInputMode(InputGameOnly);
		bIsCommonWidgetVisible = false;
	}
	else
	{
		//CommonWidget->AddToViewport(1);
		CommonWidget->SetVisibility(ESlateVisibility::Visible);
		FInputModeGameAndUI InputGameAndUI;
		SetInputMode(InputGameAndUI);
		bIsCommonWidgetVisible = true;
	}
}

// 마우스커서 토글 함수
void AK_PlayerController::ToggleMouseCursor(const FInputActionValue& value)
{
	// GetWorld() 유효 확인
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp , Error , TEXT("World is not valid in Setup."));
		return; // World가 유효하지 않으면
	}

	// PlayerController 유효 확인
	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!PlayerController)
	{
		UE_LOG(LogTemp , Error , TEXT("PlayerController is not valid in Setup."));
		return; // PlayerController가 유효하지 않으면 
	}

	if (PlayerController)
	{
		if (bIsMouseCursorShow)
		{
			FInputModeGameOnly InputGameOnlyMode;
			PlayerController->SetInputMode(InputGameOnlyMode);
			PlayerController->bShowMouseCursor = false; // 마우스 커서를 숨김
			bIsMouseCursorShow = false;
		}
		else
		{
			FInputModeGameAndUI InputGameUIMode;
			PlayerController->SetInputMode(InputGameUIMode);
			PlayerController->bShowMouseCursor = true; // 마우스 커서를 보이게
			bIsMouseCursorShow = true;
		}
	}
}

// (임시) StandbyWidget 제거 함수
void AK_PlayerController::RemoveStandbyWidget(const struct FInputActionValue& value)
{
	GEngine->AddOnScreenDebugMessage(-1 , 1.0f , FColor::Red , TEXT("Remove UI"));
	if (StandbyUI)
	{
		StandbyUI->RemoveUI();
	}
}

//클라이언트 UI생성 및 IMC맵핑 RPC함수
void AK_PlayerController::CRPC_SetIMCnCreateStandbyUI_Implementation()
{
	if (IsLocalController())
	{
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
			GetLocalPlayer());
		if (Subsystem)
		{
			if (nullptr == IMC_Viper)
			{
				UE_LOG(LogTemp , Log , TEXT("nullptr == IMC_Viper"));
			}
			else
			{
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
		if (StandbyUI && CurrentMapName == FString::Printf(TEXT("CesiumTest")))
		{
			StandbyUI->SetUI();

			if (UK_GameInstance* GI = Cast<UK_GameInstance>(GetGameInstance()))
			{
				if (StandbyUI)
				{
					StandbyUI->ClientUpdatePlayerList(GI->ConnectedPlayerNames); // StandbyUI에서 PlayerList 동기화
					if (GI->ServerWidget && !GI->bHost)
						StandbyUI->ReqMapInfo(GI->JoinRoomName);
				}
			}
			//StandbyUI->InitializeMissionData(); // 서버에서 설정한 데이터와 동기화
		}
	}
	else
	LOG_S(Warning , TEXT("Is Not Local Controller"));
}

// 클라이언트를 로비 레벨로 트래블시키는 함수
void AK_PlayerController::TravelToLobbyLevel()
{
	// 로비 맵으로 클라이언트를 이동
	ClientTravel("/Game/Maps/KHS/K_LobbyMap" , ETravelType::TRAVEL_Absolute);
}
