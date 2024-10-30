// Fill out your copyright notice in the Description page of Project Settings.


#include "KHS/K_GameInstance.h"
#include "KHS/K_LoginRegisterWidget.h"
#include "KHS/K_PlayerController.h"
#include "KHS/K_PlayerState.h"
#include "KHS/K_ServerWidget.h"
#include "KHS/K_LoadingWidget.h"
#include "KHS/K_StandbyWidget.h"
#include "KHS/K_JsonParseLib.h"
#include <MTVS_AirJet_Final.h>

#include "Components/AudioComponent.h"
#include <Engine/World.h>
#include "Engine/Engine.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "JsonObjectConverter.h"
#include "Math/UnrealMathUtility.h"
#include "Templates/SubclassOf.h"
#include "Kismet/GameplayStatics.h"

#include "../../../../Plugins/Online/OnlineSubsystem/Source/Public/Interfaces/OnlineSessionInterface.h"
#include "../../../../Plugins/Online/OnlineBase/Source/Public/Online/OnlineSessionNames.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "KHS/K_GameState.h"
#include "Net/UnrealNetwork.h"


// 세션 생성에 사용할 수 있는 세션 이름을 전역 상수로 정의
const static FName SESSION_NAME = TEXT("Session Name");
const static FName SERVER_NAME_SETTINGS_KEY = TEXT("ServerName");
const static FName SERVER_DATA_SETTINGS_KEY = TEXT("ServerData");


void UK_GameInstance::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps)
{
	DOREPLIFETIME(UK_GameInstance , ConnectedPlayerNames);
}

void UK_GameInstance::OnConnectedPlayerNames()
{
	KGameState = Cast<AK_GameState>(GetWorld()->GetGameState());
	if ( KGameState )
	{
		KGameState->SetConnectedPlayerNames(ConnectedPlayerNames);
	}
}

UK_GameInstance::UK_GameInstance(const FObjectInitializer& ObjectInitializer)
{
	UE_LOG(LogTemp , Warning , TEXT("GameInstance Constructor"));
}

void UK_GameInstance::Init()
{
	Super::Init();

	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get(); // OnlineSubsystem 가져오기

	if (Subsystem) // 만약, Subsystem이 유효하다면,
	{
		SessionInterface = Subsystem->GetSessionInterface(); // 세션 인터페이스 가져오기

		// 만약, 세션 인터페이스가 유효하다면,
		if (SessionInterface.IsValid())
		{
			//세션인터페이스의 처리상태에 따라 바인딩 함수 연결
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(
				this , &UK_GameInstance::OnCreateSessionComplete);
			SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(
				this , &UK_GameInstance::OnDestroySessionComplete);
			SessionInterface->OnFindSessionsCompleteDelegates.
			                  AddUObject(this , &UK_GameInstance::OnFindSessionComplete);
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this , &UK_GameInstance::OnJoinSessionComplete);
		}
	}
}

void UK_GameInstance::OnStart()
{
	// 처음에는 로비 사운드 재생
	PlayLobbySound();
}

// 세션 생성 완료 시 호출될 함수
void UK_GameInstance::OnCreateSessionComplete(FName SessionName , bool Success)
{
	// 세션 생성 실패 시,
	if (!Success)
	{
		if (LoadingWidget)
			LoadingWidget->RemoveUI();
		return;
	}
	GEngine->AddOnScreenDebugMessage(0 , 2 , FColor::Green , TEXT("Hosting"));

	// 세션이 성공적으로 생성 시,
	if (ServerWidget) // ServerWidget 제거
		ServerWidget->RemoveUI();
	if (LoadingWidget) // LoadingWidget 생성
		LoadingWidget->SetUI();

	// 맵 전환 전, 비동기 로딩을 시작 -> 끝나면 OnMapPreloadComplete을 호출하여 ServerTravel 시작
	StreamableManager.RequestAsyncLoad(FSoftObjectPath(TEXT("/Game/Maps/KHS/CesiumTest.CesiumTest")) ,
	                                   FStreamableDelegate::CreateUObject(
		                                   this , &UK_GameInstance::OnMapPreloadComplete));
}

// Map 비동기 load 함수
void UK_GameInstance::OnMapPreloadComplete()
{
	if (LoadingWidget)
		LoadingWidget->RemoveUI();

	StopCurrentSound(); // 기존 사운드 중지

	// 게임맵으로 리슨서버를 열고 Server Travel
	GetWorld()->ServerTravel(TEXT("/Game/Maps/KHS/CesiumTest?listen"));

	// 맵 전환 후 약간의 지연을 두고 StageSound 재생
	FTimerHandle TimerHandle_StageSound;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_StageSound , this , &UK_GameInstance::PlayStageSound , 3.0f ,
	                                       false);
}

// 세션 파괴 완료 시 호출될 바인딩 함수
void UK_GameInstance::OnDestroySessionComplete(FName SessionName , bool Success)
{
	if (Success) // 파괴에 성공하면?
	{
		CreateSession(); // (기존에 세션이 있으면 파괴하고 새로운 세션 생성)
	}

	if (nullptr != GEngine)
	{
		GEngine->OnNetworkFailure().AddUObject(this , &UK_GameInstance::OnNetworkFailure);
	}
}

// 세션 찾기 완료 시 호출될 바인딩 함수. Success 인자만 있으면됨 -> 발견된 세션 목록은 SeesionSearch TSharedRef 포인터에 있기 때문
void UK_GameInstance::OnFindSessionComplete(bool Success)
{
	if (!Success)
	{
		UE_LOG(LogTemp , Error , TEXT("OnFindSessionComplete failed: Session search failed."));
		return;
	}

	if (!SessionSearch.IsValid())
	{
		UE_LOG(LogTemp , Error , TEXT("OnFindSessionComplete failed: SessionSearch is not valid."));
		return;
	}

	if (!ServerWidget)
	{
		UE_LOG(LogTemp , Error , TEXT("OnFindSessionComplete failed: ServerWidget is not valid."));
		return;
	}

	UE_LOG(LogTemp , Warning , TEXT("Starting Find Session"));

	//ServerList에 내용 업데이트
	TArray<FServerData> ServerNames;

	int index = 0;
	for (const FOnlineSessionSearchResult& SearchResult : SessionSearch->SearchResults)
	{
		FServerData Data;
		Data.sessionIdx = index++; //결과 순서대로 인덱스 부여
		Data.sessionName = SearchResult.GetSessionIdStr();
		Data.maxPlayers = SearchResult.Session.SessionSettings.NumPublicConnections; // 입장가능한 최대 플레이어 수
		Data.curPlayers = Data.maxPlayers - SearchResult.Session.NumOpenPublicConnections;
		// 최대 플레이어 수 - 비어있는 슬롯의 수 = 접속 중인 플레이어 수
		Data.hostUserName = SearchResult.Session.OwningUserName;
		FString ServerName;
		if (SearchResult.Session.SessionSettings.Get(SERVER_NAME_SETTINGS_KEY , ServerName))
		{
			Data.sessionName = ServerName;
		}
		else
		{
			//Data.Name = "Could not Find Name";
			Data.sessionName = SearchResult.GetSessionIdStr(); // 서버 이름을 찾을 수 없는 경우, 고유 아이디를 가져옴
		}

		// 세션에 저장된 MapData를 가져와서 Map변수에 저장->ServerList에서 Index값을 가지고 있고
		// -> Index값 기준으로 ServerWidget에 접근해 위젯 SetText
		FString ServerData;
		if (SearchResult.Session.SessionSettings.Get(SERVER_DATA_SETTINGS_KEY , ServerData))
		{
			ServerDataList[Data.sessionIdx] = ServerData;
			LOG_S(Warning , TEXT("ServerData : %s") , *ServerData);
		}

		ServerNames.Add(Data);
	}

	// ServerWidget이 유효하고 SetServerList 호출이 안전한 경우에만 실행
	if (ServerWidget)
	{
		ServerWidget->SetServerList(ServerNames);
	}
	else
	{
		UE_LOG(LogTemp , Error , TEXT("ServerWidget is not valid during SetServerList call."));
	}
}

// 세션 Join 완료시 호출된 바인딩 함수	
void UK_GameInstance::OnJoinSessionComplete(FName SessionName , EOnJoinSessionCompleteResult::Type Result)
{
	FString Address;

	if (SessionInterface.IsValid())
	{
		if (!SessionInterface->GetResolvedConnectString(SessionName , Address))
		{
			UE_LOG(LogTemp , Warning , TEXT("Could Not Get Connect String"));
			return;
		}
	}

	APlayerController* PlayerController = GetFirstLocalPlayerController();
	if (PlayerController)
	{
		MyName = PlayerController->PlayerState->GetPlayerName();
		PlayerController->ClientTravel(Address , ETravelType::TRAVEL_Absolute);
	}
}

//네트워크서버 찾기 실패시 호출
void UK_GameInstance::OnNetworkFailure(UWorld* World , UNetDriver* NetDriver , ENetworkFailure::Type FailureType ,
                                       const FString& ErrorString)
{
	// 현재 사운드를 중지
	StopCurrentSound();

	// 서버 위젯 맵으로 이동
	TravelMainLobbyMap(false); // false 인자를 통해 현재 사운드를 유지하지 않으며 이동

	// 로비 사운드 재생
	PlayLobbySound();
}

//세션리스트 업데이트함수
void UK_GameInstance::RefreshServerList()
{
	// 기존 세션 검색 결과 초기화
	if (SessionSearch.IsValid())
	{
		SessionSearch.Reset();
	}

	SessionSearch = MakeShareable(new FOnlineSessionSearch());

	if (SessionSearch.IsValid())
	{
		if (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL") // OnlineSubsystem 이 NULL 로 세팅되면 (NULL : 로컬 연결 설정)
		{
			SessionSearch->bIsLanQuery = true; // LAN 사용 여부, true 면 LAN 세션을 찾고 false 면 인터넷 세션을 찾음.
		}
		else
		{
			SessionSearch->bIsLanQuery = false;
			// LAN 사용 여부, true 면 LAN 세션을 찾고 false 면 인터넷 세션을 찾음. (Steam, XBox 등 공식플랫폼 연결 설정)
		}
		SessionSearch->MaxSearchResults = 50; // 한번 세션을 찾을때 최대 50개까지만 검색
		SessionSearch->QuerySettings.Set(SEARCH_PRESENCE , true , EOnlineComparisonOp::Equals);

		SessionInterface->FindSessions(0 , SessionSearch.ToSharedRef());
		// ToSharedRef -> TSharedPtr 을 항상 유효하게 바꿔주는 내장함수. 
		// TSharedptr 은 Null일 수도 있는데, FindSession이란 메서드는 Null이면 위험하니까 애초에 유효한 녀석만 넣게 요청
		// 그래서 우리가 항상 유효하게 ToSharedRef로 변환해줘야함
	}
}

// 1) 세션 관련 함수 --------------------------------------------------------------------------------------
//서버생성함수
void UK_GameInstance::Host(FString ServerName , const FString& MapDataStruct)
{
	DesiredServerName = ServerName;
	DesiredServerData = MapDataStruct;

	// 호스트 플레이어 이름을 `ConnectedPlayerNames`에 추가
	APlayerController* PlayerController = GetFirstLocalPlayerController();
	if (PlayerController)
	{
		FString HostName = PlayerController->PlayerState->GetPlayerName();
		MyName = HostName;
		// ConnectedPlayerNames.Add(HostName); // 호스트 이름 추가
		//
		
	}

	// 만약, 세션 인터페이스가 유효하다면,
	if (SessionInterface.IsValid())
	{
		// LoadingWidget 초기화
		if (LoadingWidgetFactory)
			LoadingWidget = CreateWidget<UK_LoadingWidget>(this , LoadingWidgetFactory);

		if (LoadingWidget)
			LoadingWidget->SetUI();

		auto ExistingSession = SessionInterface->GetNamedSession(SESSION_NAME); // 현재 세션 정보 얻기
		if (ExistingSession) // 세션이 이미 존재한다면
		{
			UE_LOG(LogTemp , Warning , TEXT("Existing session found. Destroying the session..."));
			SessionInterface->DestroySession(SESSION_NAME); // 기존에 명명된 세션을 파괴
			// 실행되면 'DestroySession'이 델리게이트에 정보를 제공한다. 즉, 바로 델리게이트가 호출된다.
		}

		else // 세션이 없을 경우
		{
			UE_LOG(LogTemp , Warning , TEXT("No existing session found. Creating a new session..."));
			CreateSession(); // 새로운 세션 생성
		}
	}
}

//서버접속함수
void UK_GameInstance::Join(uint32 Index)
{
	// SessionInterface 또는 SessionSearch가 유효하지 않다면, return
	if (!SessionInterface.IsValid() || !SessionSearch.IsValid())
	{
		UE_LOG(LogTemp , Warning , TEXT("SessionInterface or SessionSearch is not valid."));
		return;
	}

	if (SessionInterface->GetNamedSession(SESSION_NAME)) // 이미 세션에 접속되어 있는지 확인
	{
		UE_LOG(LogTemp , Warning , TEXT("Already connected to a session. Cannot join again."));
		return; // 이미 접속된 세션이 있다면 다시 접속하지 않도록 방지
	}

	if (ServerWidget)
	{
		ServerWidget->RemoveUI(); //서버 UI제거
	}


	//Session Interface 를 통해 JoinSession 실행
	SessionInterface->JoinSession(0 , SESSION_NAME , SessionSearch->SearchResults[Index]);
}

//세션생성함수
void UK_GameInstance::CreateSession()
{
	if (SessionInterface.IsValid())
	{
		FOnlineSessionSettings SessionSettings; // CreateSession을 위해 임의로 세션세팅을 만들어준다.
		if (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL") // OnlineSubsystem 이 NULL 로 세팅되면 (NULL : 로컬 연결 설정)
		{
			SessionSettings.bIsLANMatch = true; // true 시 : 같은 네트워크에 있는 사람을 찾음 (로컬 연결일 때)
		}
		else
		{
			SessionSettings.bIsLANMatch = false; // false 시 : 다른 네트워크와 연결 가능하도록 함. (Steam, XBox 등 공식플랫폼 연결 설정)
		}

		LOG_S(Warning , TEXT("Input Data : %s") , *DesiredServerData);
		//Session Setting 초기화
		SessionSettings.NumPublicConnections = 5; // 플레이어 수
		SessionSettings.bShouldAdvertise = true; // 온라인에서 세션을 볼 수 있도록함.3
		SessionSettings.bUseLobbiesIfAvailable = true; // 로비기능을 활성화한다. (Host 하려면 필요)
		SessionSettings.bUsesPresence = true;
		SessionSettings.Set(SERVER_NAME_SETTINGS_KEY , DesiredServerName ,
		                    EOnlineDataAdvertisementType::ViaOnlineServiceAndPing); // ServerName 키 세팅
		SessionSettings.Set(SERVER_DATA_SETTINGS_KEY , DesiredServerData ,
		                    EOnlineDataAdvertisementType::ViaOnlineServiceAndPing); // ServerData 키 세팅


		SessionInterface->CreateSession(0 , SESSION_NAME , SessionSettings); // 세션을 생성한다. 
		// 실행되면 'CreateSession'이 델리게이트 호출 // 인자(플레이어번호, TEXT("세션이름"), 세션세팅)
	}
}


// 2) UI 생성 관련 함수 -----------------------------------------------------------------------------------
// 로그인 UI를 생성하는 함수
void UK_GameInstance::CreateLoginWidget()
{
	// LoginWidgetFactory를 통해 LogInUI 위젯 생성
	LoginWidget = CreateWidget<UK_LoginRegisterWidget>(this , LoginWidgetFactory);
	LoginWidget->SetInterface(this); //부모함수 호출
	LoginWidget->SetUI(); //부모함수 호출
}

// Server UI를 생성하는 함수
void UK_GameInstance::CreateServerWidget()
{
	// ServerUIFactory를 통해 ServerUI 위젯 생성
	ServerWidget = CreateWidget<UK_ServerWidget>(this , ServerWidgetFactory);
	ServerWidget->SetInterface(this); //부모함수 호출
	ServerWidget->SetUI(); //부모함수 호출
}

// Loading UI를 생성하는 함수
void UK_GameInstance::CreateLoadingWidget()
{
	// LoadingUIFactory를 통해 LoadingUI 위젯 생성
	LoadingWidget = CreateWidget<UK_LoadingWidget>(this , LoadingWidgetFactory);
	LoadingWidget->SetInterface(this); //부모함수 호출
	LoadingWidget->SetUI(); //부모함수 호출
}

// Standby UI를 생성하는 함수
void UK_GameInstance::CreateStandbyWidget()
{
	// LoadingUIFactory를 통해 LoadingUI 위젯 생성
	StandbyWidget = CreateWidget<UK_StandbyWidget>(this , StandbyWidgetFactory);
	StandbyWidget->SetInterface(this); //부모함수 호출
	StandbyWidget->SetUI(); //부모함수 호출
}

//미션데이터를 GameState를 통해 StandbyUI에 업데이트해주는 함수
void UK_GameInstance::InitializeMission(const FMissionDataRes& newMD)
{
	MissionData = newMD;
	MissionData.ToString();
}

// 3) Travel 관련 함수 ------------------------------------------------------------------------------------
void UK_GameInstance::TravelMainLobbyMap(bool bKeepCurrentSound)
{
	// K_PlayerController를 가져온다,
	AK_PlayerController* pc = Cast<AK_PlayerController>(GetFirstLocalPlayerController());
	if (pc && pc->IsLocalController()) // 컨트롤러가 있으면,
	{
		if (false == bKeepCurrentSound) // false 인자인 경우 기존 사운드를 유지하지 않으면서 이동
		{
			StopCurrentSound();
		}
		else
		{
			ContinueCurrentSound();
		}

		// PlayerController를 통해 LobbyMap으로 이동시킨다.
		pc->ClientTravel("/Game/Maps/KHS/K_LobbyMap" , ETravelType::TRAVEL_Absolute);
	}

	else
	{
		UE_LOG(LogTemp , Error , TEXT("Failed to get PlayerController in LoadServerWidgetMap."));
	}
}

// 5) 사운드 관련 함수 --------------------------------------------------------------------------------------
#pragma region Sound Setting

// 로비 사운드 재생 함수
void UK_GameInstance::PlayLobbySound()
{
	if (LobbySound)
	{
		if (CurrentPlayingSound)
		{
			CurrentPlayingSound->Stop();
		}
		CurrentPlayingSound = UGameplayStatics::SpawnSound2D(this , LobbySound , 1.0f , 1.0f , 0.0f , nullptr , true ,
		                                                     false);
		UE_LOG(LogTemp , Warning , TEXT("Started playing lobby sound"));
	}
	else
	{
		UE_LOG(LogTemp , Error , TEXT("LobbySound is not set"));
	}
}

// 시뮬레이션 스테이지 사운드 재생 함수
void UK_GameInstance::PlayStageSound()
{
	if (StageSound)
	{
		CurrentPlayingSound = UGameplayStatics::SpawnSound2D(this , StageSound , 1.0f , 1.0f , 0.0f);
		UE_LOG(LogTemp , Warning , TEXT("Started playing stage sound"));
	}
	else
	{
		UE_LOG(LogTemp , Error , TEXT("StageSound is not set"));
	}
}

// 사운드 재생 중지 함수
void UK_GameInstance::StopCurrentSound()
{
	if (CurrentPlayingSound && CurrentPlayingSound->IsPlaying())
	{
		CurrentPlayingSound->Stop();
		UE_LOG(LogTemp , Warning , TEXT("Stopped current sound"));
	}
}

// 현재 사운드 유지 함수(로그인시)
void UK_GameInstance::ContinueCurrentSound()
{
	// 현재 재생 중인 사운드가 있다면 계속 재생
	if (CurrentPlayingSound && !CurrentPlayingSound->IsPlaying())
	{
		CurrentPlayingSound->Play();
	}
	else if (!CurrentPlayingSound)
	{
		// 현재 재생 중인 사운드가 없다면 로비 사운드 재생
		PlayLobbySound();
	}
}


#pragma endregion


#pragma region Server Part Coordination

void UK_GameInstance::SetUserId(const FString& NewUserId)
{
	UserId = NewUserId;
}

FString UK_GameInstance::GetUserId() const
{
	return UserId;
}

#pragma region Server Request Functions

// 1. 웹 통신 요청 함수 : getworld(), 요청 타입, json string 데이터(생략가능), 게임 인스턴스(생략가능)
void UK_GameInstance::MyServerRequest(const UWorld* world , EEventType type , const FString& sendJsonData ,
                                      UK_GameInstance* gameInstance)
{
	// 1. 게임 인스턴스 정보안들어오면 캐스트
	gameInstance = Cast<UK_GameInstance>(UGameplayStatics::GetGameInstance(world));
	check(gameInstance);

	// 2. 게임 인스턴스 통해 서버에 요청 시작
	gameInstance->RequestToServerByType(type , sendJsonData);
}

// 2. 요청 타입에 따라 다른 요청 실행 : 요청 타입, json string 데이터(생략 가능)
void UK_GameInstance::RequestToServerByType(EEventType type , const FString& sendJsonData)
{
	GEngine->AddOnScreenDebugMessage(-1 , 10.f , FColor::Green ,
	                                 FString::Printf(TEXT("%s 요청 시작") , *UEnum::GetValueAsString(type)));

	// 요청 타입 에 따라 다른 요청 실행
	switch (type)
	{
	case EEventType::REGISTER:
		{
			RegisterDel.BindUObject(this , &UK_GameInstance::MyResRegister);
			RequestData(RegisterDel , sendJsonData , TEXT("auth/signup"));
			break;
		}
	case EEventType::LOGIN:
		{
			LoginDel.BindUObject(this , &UK_GameInstance::MyResLogin);

			RequestData(LoginDel , sendJsonData , TEXT("auth/login"));
			break;
		}
	case EEventType::MAPINFO:
		{
			MapInfoDel.BindUObject(this , &UK_GameInstance::MyResMapInfo);
			RequestData(MapInfoDel , sendJsonData , TEXT("map/data"));
			break;
		}
	}
}

// 3. 서버에 요청 : 반응 딜리게이트, json 데이터, url, 기본 url 사용 여부, 전송 타입(POST or GET)
void UK_GameInstance::RequestData(FServerResponseDel resDel , const FString& jsonData , const FString& url ,
                                  bool useDefaultURL , EReqType type)
{
	// 3-1) 통신 모듈 생성
	FHttpModule* http = &FHttpModule::Get();
	// 3-2) 요청 정보 설정
	TSharedRef<IHttpRequest , ESPMode::ThreadSafe> httpReq = http->CreateRequest();
	// 3-3) 응답 함수 연결( 람다로 연결해서 정해놓은 반응 딜리게이트로 연결되도록 )
	httpReq->OnProcessRequestComplete().BindLambda(
		[resDel , this](FHttpRequestPtr req , FHttpResponsePtr res , bool isSuccess)
		{
			// 성공 여부 확인
			if (isSuccess)
			{
				// 로그인 시에만 작동, 인증 값 저장해두기
				authorValue = res->GetHeader(TEXT("Authorization"));
				// GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("인증 값 : %s"), *authorValue));
				// json string 가져오기
				FString resStr = res->GetContentAsString();
				// 딜리게이트에 바인드된 반응 함수 실행 
				resDel.ExecuteIfBound(resStr , isSuccess);
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1 , 3.f , FColor::Green , TEXT("요청 실패"));
			}
		});
	// 3-5) 기본 url 사용 여부에 따라 url 설정
	FString resultURL = useDefaultURL ? defaultURL + url : url;
	httpReq->SetURL(resultURL);
	// 3-6) 전송 타입을 설정 POST && 보낼 jsondata가 있다면 넣기
	if (type == EReqType::POST && !jsonData.IsEmpty())
		httpReq->SetContentAsString(jsonData);
	// 3-7) 요청 타입 설정 : 요청타입맵[enum] = TEXT("POST or GET")
	httpReq->SetVerb(ReqTypeMap[type]);
	// 3-8) 헤더 설정
	httpReq->SetHeader(TEXT("Content-Type") , TEXT("application/json"));
	// 3-9) 헤더에 인증값 설정 -> 로그인 이후 지속.
	httpReq->SetHeader(TEXT("Authorization") , authorValue);
	// 3-10) 서버 요청
	httpReq->ProcessRequest();
}

#pragma endregion

#pragma region Response CallBack Delegate Functions

//회원가입 요청 콜백 바인딩 함수
void UK_GameInstance::MyResRegister(const FString& jsonData , bool isSuccess)
{
	// json 데이터 -> 구조체로 변경
	FRegisterResponse resData;
	FJsonObjectConverter::JsonObjectStringToUStruct(jsonData , &resData , 0 , 0);

	GEngine->AddOnScreenDebugMessage(-1 , 31.f , FColor::Yellow ,
	                                 FString::Printf(TEXT("%s") , *resData.ResponseToString()));

	// 바인드된 함수 실행(BroadCast)
	RegisterResUseDel.ExecuteIfBound(resData);
}

//로그인 요청 콜백 바인딩 함수
void UK_GameInstance::MyResLogin(const FString& jsonData , bool isSuccess)
{
	// json 데이터 -> 구조체로 변경
	FLoginResponse resData;
	FJsonObjectConverter::JsonObjectStringToUStruct(jsonData , &resData , 0 , 0);

	GEngine->AddOnScreenDebugMessage(-1 , 31.f , FColor::Yellow ,
	                                 FString::Printf(TEXT("%s") , *resData.ResponseToString()));

	// 바인드된 함수 실행(BroadCast)
	LoginResUseDel.ExecuteIfBound(resData);
}

//맵정보 요청 콜백 바인딩 함수
void UK_GameInstance::MyResMapInfo(const FString& jsonData , bool isSuccess)
{
	FMapInfoResponse resData;
	FJsonObjectConverter::JsonObjectStringToUStruct(jsonData , &resData , 0 , 0);

	// 바인드된 함수 실행(BroadCast)
	MapInfoResUseDel.ExecuteIfBound(resData);
}
#pragma endregion


#pragma endregion
