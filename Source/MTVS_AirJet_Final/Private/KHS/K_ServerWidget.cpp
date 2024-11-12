// Fill out your copyright notice in the Description page of Project Settings.


#include "KHS/K_ServerWidget.h"
#include "KHS/K_GameInstance.h"
#include "KHS/K_WidgetBase.h"
#include "KHS/K_ServerList.h"
#include "KHS/K_PlayerList.h"
#include "KHS/K_PlayerController.h"
#include "KHS/K_JsonParseLib.h"
#include <MTVS_AirJet_Final.h>

#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Components/Widget.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "../../../../Plugins/Runtime/WebBrowserWidget/Source/WebBrowserWidget/Public/WebBrowser.h"

#include "UObject/ConstructorHelpers.h"
#include "Engine/Texture2D.h"
#include "ImageUtils.h"
#include "Kismet/GameplayStatics.h"

#pragma region Initialize Settings

UK_ServerWidget::UK_ServerWidget(const FObjectInitializer& ObjectInitialize)
{
}

void UK_ServerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ShowServerMenuAnim)
	{
		// 메인 애니메이션 재생
		//PlayAnimation(ShowTransitionAnim);
		PlayAnimation(ShowServerMenuAnim);
	}

	if (WidgetInterface)
		WidgetInterface->RefreshServerList();

	// GameInstance 가져오기
	GameInstance = Cast<UK_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (!GameInstance)
	{
		UE_LOG(LogTemp , Error , TEXT("GameInstance is null in UK_ServerWidget"));
	}
}

bool UK_ServerWidget::Initialize()
{
	Super::Initialize();

	// Server Menu Button Binding =============================================
	if (ServerMenu_btn_Create) //방생성 버튼 바인딩
	{
		ServerMenu_btn_Create->OnClicked.AddDynamic(this , &UK_ServerWidget::OpenHostMenuFromServer);
	}
	if (ServerMenu_btn_Join) //방참여 버튼 바인딩
	{
		ServerMenu_btn_Join->OnClicked.AddDynamic(this , &UK_ServerWidget::JoinRoom);
	}
	if (ServerMenu_btn_Home) //홈 버튼 바인딩
	{
		ServerMenu_btn_Home->OnClicked.AddDynamic(this , &UK_ServerWidget::OpenLobbyLevel);
	}
	if (ServerMenu_btn_Reset) //리셋 버튼 바인딩
	{
		ServerMenu_btn_Reset->OnClicked.AddDynamic(this , &UK_ServerWidget::ServerListUpdateChildren);
	}
	// Host Menu Button Binding ==============================================
	if (HostMenu_btn_Create) //방생성 버튼 바인딩
	{
		HostMenu_btn_Create->OnClicked.AddDynamic(this , &UK_ServerWidget::CreateRoom);
	}
	// if (HostMenu_btn_LoadMap) //웹에디터 정보 불러오기 버튼 바인딩
	// {
	// 	HostMenu_btn_LoadMap->OnClicked.AddDynamic(this , &UK_ServerWidget::OpenCreaterWeb);
	// }
	if (HostMenu_btn_Cancel) //생성취소 버튼 바인딩
	{
		HostMenu_btn_Cancel->OnClicked.AddDynamic(this , &UK_ServerWidget::OpenServerMenuFromHost);
	}
	// if (HostMenu_btn_WebQuit) //웹에디터 종료 버튼 바인딩
	// {
	// 	HostMenu_btn_WebQuit->OnClicked.AddDynamic(this , &UK_ServerWidget::QuitCreaterWeb);
	// }


	return true;
}

#pragma endregion

// Common =======================================================

#pragma region Common Functions

// 서버메뉴로 돌아가는 함수
void UK_ServerWidget::OpenServerMenuFromHost()
{
	if (HideHostMenuAnim)
		PlayAnimation(HideHostMenuAnim);

	//ButtonClickSound를 재생하고싶다.
	UGameplayStatics::PlaySound2D(GetWorld() , ButtonClickSound);
	
	// 타이머로 시간 제어를 통해 ServerMenu 전환 및 전환 애니메이션 실행
	FTimerHandle TimerHandle_MenuSwitch;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_MenuSwitch , [this]()
	{
		if (MenuSwitcher)
		{
			MenuSwitcher->SetActiveWidget(ServerMenu); // ServerMenu로 전환하여 활성화
			PlayAnimation(ShowServerMenuAnim);
			UE_LOG(LogTemp , Warning , TEXT("ServerMenu is Activate"));

			if (WidgetInterface)
				WidgetInterface->RefreshServerList();
		}
	} , 1.0f , false);
}

// 서버메뉴로 돌아가는 함수
// void UK_ServerWidget::OpenServerMenuFromReady()
// {
// 	// 타이머로 시간 제어를 통해 ServerMenu 전환 및 전환 애니메이션 실행
// 	FTimerHandle TimerHandle_MenuSwitch;
// 	GetWorld()->GetTimerManager().SetTimer(TimerHandle_MenuSwitch , [this]()
// 	{
// 		if (MenuSwitcher)
// 		{
// 			MenuSwitcher->SetActiveWidget(ServerMenu); // ServerMenu로 전환하여 활성화
// 			PlayAnimation(ShowServerMenuAnim);
// 			UE_LOG(LogTemp , Warning , TEXT("ServerMenu is Activate"));
//
// 			if (WidgetInterface)
// 				WidgetInterface->RefreshServerList();
// 		}
// 	} , 1.0f , false);
// }

// Host 메뉴로 접속하는 함수
void UK_ServerWidget::OpenHostMenuFromServer()
{
	if (HideServerMenuAnim)
		PlayAnimation(HideServerMenuAnim);

	//ButtonClickSound를 재생하고싶다.
	UGameplayStatics::PlaySound2D(GetWorld() , ButtonClickSound);
	
	// 타이머로 시간 제어를 통해 HostMenu 전환 및 전환 애니메이션 실행
	FTimerHandle TimerHandle_MenuSwitch;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_MenuSwitch , [this]()
	{
		if (MenuSwitcher)
		{
			MenuSwitcher->SetActiveWidget(HostMenu); // HostMenu로 전환하여 활성화

			//시작하자마자 WebURL실행
			OpenCreaterWeb();
			//숨겨둬야하는 위젯들 설정(변경)
			// HostMenu_web_Popup->SetVisibility(ESlateVisibility::Hidden);
			// HostMenu_btn_WebQuit->SetVisibility(ESlateVisibility::Hidden);

			PlayAnimation(ShowHostMenuAnim);
			UE_LOG(LogTemp , Warning , TEXT("ServerMenu is Activate"));
		}
	} , 1.0f , false);
}

// Lobby 맵으로 돌아가는 함수
void UK_ServerWidget::OpenLobbyLevel()
{
	//ButtonClickSound를 재생하고싶다.
	UGameplayStatics::PlaySound2D(GetWorld() , ButtonClickSound);
	
	this->RemoveUI();
}

#pragma endregion

// Server Menu ==================================================

#pragma region ServerMenu Functions

// 상단에 선언된 FServerData 구조체를 바탕으로 Session 목록을 설정하는 함수
void UK_ServerWidget::SetServerList(TArray<FServerData> ServerNames)
{
	//기존목록을 지우고
	ServerMenu_ServerList->ClearChildren();

	//uint32 i = 0;
	for (const FServerData& ServerData : ServerNames)
	{
		//팩토리를 통해 ServerListUI버튼 생성
		ServerList = CreateWidget<UK_ServerList>(this , ServerListFactory);

		//ServerList SetText
		ServerList->sessionIdx->SetText(FText::FromString(FString::Printf(TEXT("%d") , ServerData.sessionIdx)));
		ServerList->SessionName->SetText(FText::FromString(ServerData.sessionName));
		ServerList->HostName->SetText(FText::FromString(ServerData.hostUserName));
		ServerList->ConnectedPlayer->SetText(
			FText::FromString(FString::Printf(TEXT("%d/%d") , ServerData.curPlayers , ServerData.maxPlayers)));

		//ServerListUI 인덱스 부여
		ServerList->Setup(this , ServerData.sessionIdx);
		//ServerList->Setup(this , i);
		//i++;

		//ServerListUI버튼추가
		ServerMenu_ServerList->AddChild(ServerList);
	}
}

// 서버의 인덱스를 선택하는 함수
void UK_ServerWidget::SelecetIndex(int Index)
{
	SelectedIndex = Index;
	ServerListUpdateChildren();
}


//ServerList업데이트(PanelWidget 내장기능 사용)
void UK_ServerWidget::ServerListUpdateChildren()
{
	// //ButtonClickSound를 재생하고싶다.
	// UGameplayStatics::PlaySound2D(GetWorld() , ButtonClickSound);

	if (WidgetInterface)
		WidgetInterface->RefreshServerList();
	
	for (int32 i = 0; i < ServerMenu_ServerList->GetChildrenCount(); ++i)
	{
		UK_ServerList* List = Cast<UK_ServerList>(ServerMenu_ServerList->GetChildAt(i));
		if (List)
		{
			List->Selected = (SelectedIndex.IsSet() && SelectedIndex.GetValue() == i);
		}
	}
}

//선택한 인덱스의 세션정보에서 MapData정보를 얻어오는 함수
void UK_ServerWidget::ReqSessionInfo(const FMapInfoRequest& mapName)
{
	// 게임 인스턴스 가져와서 만들어둔 딜리게이트에 내 함수 바인딩
	auto* gi = UK_JsonParseLib::GetKGameInstance(GetWorld());
	gi->MapInfoResUseDel.BindUObject(this , &UK_ServerWidget::OnResSessionInfo);

	FMapInfoRequest data = mapName;

	// 서버에 요청 시작 -> 1~4 단계를 거쳐 바인드한 함수에 데이터가 들어옴.
	UK_GameInstance::MyServerRequest<FMapInfoRequest>(GetWorld() , EEventType::MAPINFO , data);
}

//FMapData구조체로 맵정보 란에 정보를 업데이트 하는 함수
void UK_ServerWidget::OnResSessionInfo(const FMapInfoResponse& resData)
{
	ServerMenu_txt_Producer->SetText(FText::FromString(resData.producer));
	ServerMenu_txt_MapName->SetText(FText::FromString(resData.mapName));

	//ReadyMenu Image 세팅
	FString ImgDataString = resData.mapImage;
	TArray<uint8> MapImgData;
	// Base64 문자열을 디코딩하여 TArray<uint8>에 저장
	if (FBase64::Decode(ImgDataString , MapImgData))
	{
		// 이미지 데이터를 Texture2D로 변환
		UTexture2D* imgTexture = FImageUtils::ImportBufferAsTexture2D(MapImgData);
		if (imgTexture && ServerMenu_img_Map)
		{
			ServerMenu_img_Map->SetBrushFromTexture(imgTexture); // UI에 적용
		}
		else
		{
			LOG_S(Warning , TEXT("Can not find Texture"));
		}
	}
	else
	{
		LOG_S(Warning , TEXT("Failed to decode Base64 string"));
	}
}


// 생성된 Session에 접속하는 함수
void UK_ServerWidget::JoinRoom()
{
	//ButtonClickSound를 재생하고싶다.
	UGameplayStatics::PlaySound2D(GetWorld() , ButtonClickSound);
	
	//인덱스가 Set되고, Interface클래스 변수가 부모에 존재하면
	if (SelectedIndex.IsSet() && WidgetInterface != nullptr)
	{
		//WidgetBase를 통해 Interface의 Join 가상함수 호출 -> GameInstance에 있는 구현부가 호출됨
		WidgetInterface->Join(SelectedIndex.GetValue());
		UE_LOG(LogTemp , Warning , TEXT("Selected Index is %d.") , SelectedIndex.GetValue());
	}

	else
	{
		UE_LOG(LogTemp , Warning , TEXT("Selected Index not set"));
	}
}


#pragma endregion

// Host Menu ====================================================

#pragma region HostMenu Functions


// 크리에이터툴 웹서비스로 접속하는 함수
void UK_ServerWidget::OpenCreaterWeb()
{
	// 웹 브라우저 팝업을 열고 특정 URL을 설정
	FString URL = TEXT("http://125.132.216.190:7759/"); // 원하는 URL 입력
	if (HostMenu_web_Popup)
	{
		HostMenu_web_Popup->LoadURL(URL);
		//HostMenu_web_Popup->SetVisibility(ESlateVisibility::Visible);
		//HostMenu_btn_WebQuit->SetVisibility(ESlateVisibility::Visible);
	}
}

// 웹브라우저 위젯 종료 함수
// void UK_ServerWidget::QuitCreaterWeb()
// {
// 	if (HostMenu_web_Popup)
// 	{
// 		HostMenu_web_Popup->SetVisibility(ESlateVisibility::Hidden);
// 		HostMenu_btn_WebQuit->SetVisibility(ESlateVisibility::Hidden);
// 	}
// }

// (현재) Interface에서 Host 함수를 호출하는 함수 (Origin) ReadyMenu로 정보를 가진채 넘어가기.
void UK_ServerWidget::CreateRoom()
{
	//Editable Text에 적은 MapName기준으로 백엔드서버에 요청하고
	//콜백함수에 Create Room관련 내용들을 설정(정보를 받아두고 세션을 열기 위함)
	ReqMapInfo();

	//ButtonClickSound를 재생하고싶다.
	UGameplayStatics::PlaySound2D(GetWorld() , ButtonClickSound);
	
	//// Interface에서 Host 함수를 호출 -> GameInstance에 있는 Host함수를 작동
	//if ( SessionInterface )
	//{
	//	FString ServerName = HostMenu_txt_RoomName->GetText().ToString();
	//	SessionInterface->Host(ServerName , CreatedMapData);
	//}
}

//mapName입력내용 기준으로 서버에 요청하는 함수
void UK_ServerWidget::ReqMapInfo()
{
	// 게임 인스턴스 가져와서 만들어둔 딜리게이트에 내 함수 바인딩
	auto* gi = UK_JsonParseLib::GetKGameInstance(GetWorld());

	if (!gi->MapInfoResUseDel.IsBound())
		gi->MapInfoResUseDel.BindUObject(this , &UK_ServerWidget::ResMapInfo);
	//->이 델리게이트 바인딩을 통해 GameInstance에서 콜백이 들어올떄 
	//  이 델리게이트 변수가 BroadCast되면 이곳의 연결함수가 실행

	FMapInfoRequest data;
	data.mapName = HostMenu_txt_LoadedMapName->GetText().ToString();

	// 서버에 요청 시작 -> 1~4 단계를 거쳐 바인드한 함수에 데이터가 들어옴.
	UK_GameInstance::MyServerRequest<FMapInfoRequest>(GetWorld() , EEventType::MAPINFO , data);
}

//서버요청 콜백 바인딩_해당 맵정보를 받아와 Ready MENU와 Server MENU에 세팅하는 함수
void UK_ServerWidget::ResMapInfo(const FMapInfoResponse& resData)
{
	GEngine->AddOnScreenDebugMessage(-1 , 31.f , FColor::Yellow ,
	                                 FString::Printf(
		                                 TEXT("MapInfo Requset Call Back Data \n%s") , *resData.ResponseToString()));

	CreatedMapData = resData.ResponseToServerData();

	// 정보를 받아온 것이 도착해야(ResMapInfo가 실행되야) Host가 시작되도록 함.
	// Interface에서 Host 함수를 호출 -> GameInstance에 있는 Host함수를 작동
	if (WidgetInterface)
	{
		FString ServerName = HostMenu_txt_RoomName->GetText().ToString();
		WidgetInterface->Host(ServerName , CreatedMapData);
	}

	//인게임에서 사용할 미션데이터를 인스턴스에 저장
	FMissionDataRes md;
	md.producer = resData.producer;
	md.latitude = resData.latitude;
	md.longitude = resData.longitude;
	md.mapName = resData.mapName;
	md.mapImage = resData.mapImage;
	md.startPoint.x = resData.startPointX;
	md.startPoint.y = resData.startPointY;
	md.mission = resData.mission;

	GameInstance->InitializeMission(md);
}
#pragma endregion
