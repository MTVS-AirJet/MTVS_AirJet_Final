// Fill out your copyright notice in the Description page of Project Settings.


#include "KHS/K_StreamingUI.h"
#include "KHS/K_StreamingActor.h"
#include "KHS/K_SharingUserSlot.h"
#include "MTVS_AirJet_FinalCharacter.h"

#include "EngineUtils.h"
#include "Components/UniformGridPanel.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/SceneCaptureComponent2D.h"
#include "../../../../Plugins/Media/PixelStreaming/Source/PixelStreaming/Public/IPixelStreamingModule.h"
#include "../../../../Plugins/Media/PixelStreaming/Source/PixelStreamingInput/Public/PixelStreamingInputEnums.h"
#include "../../../../Plugins/Media/PixelStreaming/Source/PixelStreaming/Public/PixelStreamingVideoInputRenderTarget.h"

#include "../../../../Plugins/Online/OnlineSubsystem/Source/Public/OnlineSubsystem.h"
#include "../../../../Plugins/Online/OnlineSubsystem/Source/Public/Interfaces/OnlineSessionInterface.h"
#include "../../../../Plugins/Online/OnlineSubsystem/Source/Public/OnlineSessionSettings.h"
#include "Kismet/GameplayStatics.h"


// 위젯이 생성된 후 초기화할 때 호출되는 함수
void UK_StreamingUI::NativeConstruct()
{
	// 레벨에 배치된 ScreenActor를 찾음
	for ( TActorIterator<AK_StreamingActor> It(GetWorld() , AK_StreamingActor::StaticClass()); It; ++It )
	{
		ScreenActor = *It;
		break;
	}

	ButtonLookSharingScreen->OnClicked.AddDynamic(this , &UK_StreamingUI::OnButtonLookSharingScreen);
	ButtonWindowScreen->OnClicked.AddDynamic(this , &UK_StreamingUI::OnButtonWindowScreen);
	ImageSharingScreen->SetVisibility(ESlateVisibility::Hidden);
	//ImageCoveringScreen->SetVisibility(ESlateVisibility::Hidden);

	Me = Cast<AMTVS_AirJet_FinalCharacter>(GetOwningPlayerPawn());
	if ( Me )
	{
		UE_LOG(LogTemp , Warning , TEXT("Me is not Null"));
		Me->StreamingUI->TextWindowScreen->SetText(FText::FromString(TEXT("Screen Share")));
	}
	else
	{
		UE_LOG(LogTemp , Warning , TEXT("Me is NullPtr"));
	}
}

// 매 프레임마다 호출되는 함수
void UK_StreamingUI::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry , InDeltaTime);
	if ( bStreaming && nullptr != ScreenActor )
	{
		ScreenActor->UpdateTexture();
	}
}

// 위젯이 처음으로 초기화될 때 호출되는 함수
void UK_StreamingUI::NativeOnInitialized()
{
}

// 현재 실행 중인 창에 대한 프로세스 목록을 그리드 패널에 추가하는 함수
void UK_StreamingUI::NativeDestruct()
{
	if ( CurrentStreamer ) {
		CurrentStreamer->SetVideoInput(nullptr);
	}
}

// 주어진 ID를 사용하여 StreamingActor의 공유 사용자 ID를 설정
void UK_StreamingUI::SetUserID(FString ID, const bool& bAddPlayer)
{
	ScreenActor->SetViewSharingUserID(ID , bAddPlayer);
}

// 화면 공유 버튼을 눌렀을 때, 호출될 델리게이트에 등록할 함수
void UK_StreamingUI::OnButtonWindowScreen()
{
	bStreaming = !bStreaming;
	//bStreaming = true;
	FString streamID; //유저아이디를 받아와서 streamID에 넣기

	if ( bStreaming )
	{
		TextWindowScreen->SetText(FText::FromString(TEXT("Sharing"))); //공유중

		if ( ScreenActor )
		{
			ScreenActor->WindowScreenPlaneMesh->SetVisibility(true);
			SetUserID(streamID , true);
		}
		else
		{
			UE_LOG(LogTemp , Error , TEXT("ScreenActor nullptr"));
		}

		streamID = GetCurrentSessionID(); //세션 아이디 받아오기

		IPixelStreamingModule& PixelStreamingModule1 = FModuleManager::LoadModuleChecked<IPixelStreamingModule>("PixelStreaming");
		CurrentStreamer = PixelStreamingModule1.CreateStreamer(streamID);
		if ( nullptr == CurrentStreamer )
			return;

		//ScreenActor에 CurrentStreamer 값 설정
		ScreenActor->CurrentStreamer = CurrentStreamer;
		//UserID로 StreamID 설정 (FString 타입)
		ScreenActor->UserID = streamID;


		////Back Buffer를 비디오 입력으로 설정합니다.
		CurrentStreamer->SetInputHandlerType(EPixelStreamingInputType::RouteToWidget);
		ScreenActor->SceneCapture->Activate();

		SetUserID(streamID , true);

		//// 2. Pixel Streaming 비디오 입력으로 설정
		VideoInput = FPixelStreamingVideoInputRenderTarget::Create(ScreenActor->SceneCapture->TextureTarget);
		CurrentStreamer->SetVideoInput(VideoInput); // 스트리밍에 사용
		CurrentStreamer->SetSignallingServerURL("ws://125.132.216.190:7755");
		CurrentStreamer->StartStreaming();
	}
	else
	{
		TextWindowScreen->SetText(FText::FromString(TEXT("Screen Share"))); //화면 공유
		ScreenActor->WindowScreenPlaneMesh->SetVisibility(false);
		SetUserID(streamID , false);

		//ProcessList->ClearChildren();

		//1. PixelStreaming 모듈을 가져옵니다.
		IPixelStreamingModule* PixelStreamingModule = FModuleManager::GetModulePtr<IPixelStreamingModule>(
			"PixelStreaming");

		if ( PixelStreamingModule )
		{
			// 2. 스트리머를 가져옵니다.
			TSharedPtr<IPixelStreamingStreamer> Streamer = PixelStreamingModule->FindStreamer(streamID);

			if ( Streamer.IsValid() )
			{
				// 4. 스트리밍을 시작합니다.
				Streamer->StopStreaming();
			}
			else
			{
				UE_LOG(LogTemp , Error , TEXT("Could not find a valid streamer with the given ID."));
			}
		}
		else
		{
			UE_LOG(LogTemp , Error , TEXT("PixelStreamingModule is not available."));
		}
	}
}

// 다른 사용자의 화면을 볼 때 호출되는 함수
void UK_StreamingUI::OnButtonLookSharingScreen()
{
	bLookStreaming = !bLookStreaming;
	if ( bLookStreaming )
	{
		ScreenActor->userID = "No Session Found";
		// 레벨에 배치된 ScreenActor를 찾음
		for ( TActorIterator<AK_StreamingActor> It(GetWorld() , AK_StreamingActor::StaticClass()); It; ++It )
		{
			ScreenActor = *It;
			break;
		}
		ImageSharingScreen->SetVisibility(ESlateVisibility::Visible);
		//블루프린트 subs
		ScreenActor->ChangeLookSharingScreen();
	}
	else
	{
		ImageSharingScreen->SetVisibility(ESlateVisibility::Hidden);
		//블루프린트 subs
		ScreenActor->StopLookSharingScreen();
		WindowList->ClearChildren();
	}
}

// StreamingActor를 설정하는 함수
void UK_StreamingUI::SetScreenActor(AK_StreamingActor* Actor)
{
	if ( !Actor )
	{
		UE_LOG(LogTemp , Error , TEXT("Invalid ScreenActor passed to SetScreenActor"));
		return;
	}

	ScreenActor = Actor;
	UE_LOG(LogTemp , Log , TEXT("ScreenActor has been set successfully."));
}

// 현재 온라인 세션의 ID를 반환하는 함수
FString UK_StreamingUI::GetCurrentSessionID()
{
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if ( OnlineSubsystem )
	{
		IOnlineSessionPtr SessionInterface = OnlineSubsystem->GetSessionInterface();
		if ( SessionInterface.IsValid() )
		{
			// "GameSession"은 기본 세션 이름이며, 필요에 따라 다른 세션 이름을 사용할 수 있음
			FNamedOnlineSession* NamedSession = SessionInterface->GetNamedSession(NAME_GameSession);
			if ( NamedSession )
			{
				// 세션 ID 가져오기
				return NamedSession->GetSessionIdStr();
			}
		}
	}

	// 세션이 없거나 가져오지 못했을 때
	return FString("No Session Found");
}

// 사용자 ID 목록을 받아 여러 사용자 슬롯을 초기화하는 함수
void UK_StreamingUI::InitSlot(TArray<FString> Items)
{
	// 기존 슬롯 제거
	WindowList->ClearChildren();
	int32 Row = 0;
	int32 Column = 0;


	// 아이템 데이터 바탕으로 슬롯 생성 및 추가
	for ( FString UserID : Items )
	{
		SharingUserSlot = CastChecked<UK_SharingUserSlot>(CreateWidget(GetWorld() , SharingUserSlotFactory));
		if ( SharingUserSlot )
		{
			// 슬롯 가시성 및 레이아웃 확인
			SharingUserSlot->SetVisibility(ESlateVisibility::Visible);
			SharingUserSlot->SetUserID(UserID);
			//SharingUserSlot->FUserIDButtonDelegate_OneParam.BindUFunction(this, FName("SetUserID"));
			// Grid에 슬롯 추가
			WindowList->AddChildToUniformGrid(SharingUserSlot , Row , Column);

			// Row 값 증가
			Row++;

			if ( !WindowList )
			{
				UE_LOG(LogTemp , Error , TEXT("PartsPanel is not valid."));
				return;
			}

			//SharingUserSlot->clickcnt = P_clickcnt; // 클릭 값 전달 (계속 InvSlot 갱신돼서 clickcnt값 업데이트 안 되는 문제 때문)
		}
	}
}

