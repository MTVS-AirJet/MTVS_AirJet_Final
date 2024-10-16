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
	ButtonLookSharingScreen->SetVisibility(ESlateVisibility::Hidden);
	ImageSharingScreen->SetVisibility(ESlateVisibility::Hidden);

	Me = Cast<AMTVS_AirJet_FinalCharacter>(GetOwningPlayerPawn());
	if ( Me )
	{
		UE_LOG(LogTemp , Warning , TEXT("Player is not Null"));
		Me->StreamingUI->TextWindowScreen->SetText(FText::FromString(TEXT("Screen Share")));
	}
	else
	{
		UE_LOG(LogTemp , Warning , TEXT("Playeris NullPtr"));
	}
}

// 매 프레임마다 호출되는 함수
void UK_StreamingUI::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry , InDeltaTime);
	if ( bStreaming && nullptr != ScreenActor )
	{
		//StreamingActor의 UpdateTexture를 UI에서 호출하여 동적매터리얼의 브러시 업데이트
		ScreenActor->UpdateTexture();
	}
}

// 위젯이 처음으로 초기화될 때 호출
void UK_StreamingUI::NativeOnInitialized()
{
}

// UI가 Destruct될때 호출
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
	
	FString streamID; //유저아이디를 받아와서 streamID에 넣을 예정

	if ( bStreaming )
	{
		TextWindowScreen->SetText(FText::FromString(TEXT("Sharing"))); //공유상태임을 나타냄

		streamID = GetCurrentSessionID(); //세션 아이디 받아오기

		// if ( ScreenActor )
		// {
		// 	//ScreenActor->WindowScreenPlaneMesh->SetVisibility(true); //송출되는 자기화면 확인용
		// 	SetUserID(streamID , true);
		// }
		// else
		// {
		// 	UE_LOG(LogTemp , Error , TEXT("ScreenActor nullptr"));
		// }

		//streamID = GetCurrentSessionID(); //세션 아이디 받아오기(위치변경)

		//픽셀스트리밍 모듈을 활성화
		IPixelStreamingModule& PixelStreamingModule1 = FModuleManager::LoadModuleChecked<IPixelStreamingModule>("PixelStreaming");
		//모듈에 StreamID를 통해 스트리머 생성 하여 픽셀스트리머로서 등록
		CurrentStreamer = PixelStreamingModule1.CreateStreamer(streamID);
		if ( nullptr == CurrentStreamer )
			return; //없으면 리턴

		//ScreenActor의 CurrentStreamer 변수에 픽셀스트리머 모듈 CurrentStreamer 값 설정
		ScreenActor->CurrentStreamer = CurrentStreamer;
		//ScreenActor의 UserID에 StreamID 설정 (FString 타입)
		ScreenActor->UserID = streamID;


		//Back Buffer를 비디오 입력으로 설정
		CurrentStreamer->SetInputHandlerType(EPixelStreamingInputType::RouteToWidget);
		//UI에서 ScreenActor의 SceneCapture를 활성화.
		ScreenActor->SceneCapture->Activate();

		//StreamID로 UserID Setting
		SetUserID(streamID , true);

		//Pixel Streaming 비디오 입력으로 설정
		VideoInput = FPixelStreamingVideoInputRenderTarget::Create(ScreenActor->SceneCapture->TextureTarget);
		CurrentStreamer->SetVideoInput(VideoInput); // 스트리밍에 사용
		CurrentStreamer->SetSignallingServerURL("ws://125.132.216.190:7755"); // 수신용 프로토콜 URL
		CurrentStreamer->StartStreaming(); // 스트리밍 시작
	}
	else
	{
		//화면공유가 끝나면
		TextWindowScreen->SetText(FText::FromString(TEXT("Screen Share"))); //방송가능상태 표시
		ScreenActor->StreamingScreen->SetVisibility(false);
		SetUserID(streamID , false); //false값 전달

		//PixelStreaming 모듈을 다시 가져오고.
		IPixelStreamingModule* PixelStreamingModule = FModuleManager::GetModulePtr<IPixelStreamingModule>(
			"PixelStreaming");

		if ( PixelStreamingModule )
		{
			//스트리머를 가져와서
			TSharedPtr<IPixelStreamingStreamer> Streamer = PixelStreamingModule->FindStreamer(streamID);

			if ( Streamer.IsValid() )
			{
				// 스트리밍을 종료.
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
		TextLookSharingScreen->SetText(FText::FromString(TEXT("Watching"))); //공유상태임을 나타냄
		//=======임시로 userID를 직접 설정해서 세션없는 상태에서 작동상태 확인가능하도록 함.
		//이후에 세션 생기면 세션정보 받아와서 userID에 변수형태로 넣으면됨(GetCurrentSessionID 함수있으니 사용바람)
		//ScreenActor->userID = "No Session Found";
		ScreenActor->UserID = GetCurrentSessionID(); //세션 아이디 받아오기 

		// 레벨에 배치된 ScreenActor를 찾음
		for ( TActorIterator<AK_StreamingActor> It(GetWorld() , AK_StreamingActor::StaticClass()); It; ++It )
		{
			ScreenActor = *It;
			break;
		}
		//화면공유 브러시가 그려질 위젯 활성화
		ImageSharingScreen->SetVisibility(ESlateVisibility::Visible);
		//블루프린트 함수 호출(송신할 스트리머 변경)
		ScreenActor->ChangeLookSharingScreen();
	}
	else
	{
		TextLookSharingScreen->SetText(FText::FromString(TEXT("Look Screen")));
		//스트리밍을 끄면 위젯 비활성화
		ImageSharingScreen->SetVisibility(ESlateVisibility::Hidden);
		//블루프린트 함수호출(스트리밍 종료, WindowList위젯의 버튼들 모두 삭제)
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

	// 세션이 없거나 가져오지 못했을 때 -> 임시로 체크할때 이걸로 확인중
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
			// Grid에 슬롯 추가
			WindowList->AddChildToUniformGrid(SharingUserSlot , Row , Column);

			// Row 값 증가
			Row++;

			if ( !WindowList )
			{
				UE_LOG(LogTemp , Error , TEXT("PartsPanel is not valid."));
				return;
			}

		}
	}
}

