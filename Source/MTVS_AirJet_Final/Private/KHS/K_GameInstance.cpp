// Fill out your copyright notice in the Description page of Project Settings.


#include "KHS/K_GameInstance.h"
#include "KHS/K_LoginRegisterWidget.h"
#include "KHS/K_PlayerController.h"
#include "KHS/K_ServerWidget.h"
#include "KHS/K_JsonParseLib.h"

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


// 세션 생성에 사용할 수 있는 세션 이름을 전역 상수로 정의
const static FName SESSION_NAME = TEXT("Session Name");
const static FName SERVER_NAME_SETTINGS_KEY = TEXT("ServerName");

// 1) 세션 관련 함수 --------------------------------------------------------------------------------------
//서버생성함수
void UK_GameInstance::Host(FString ServerName)
{
}
//서버접속함수
void UK_GameInstance::Join(uint32 Index)
{
	// SessionInterface 또는 SessionSearch가 유효하지 않다면, return
	if ( !SessionInterface.IsValid() || !SessionSearch.IsValid() )
	{
		UE_LOG(LogTemp , Warning , TEXT("SessionInterface or SessionSearch is not valid."));
		return;
	}

	if ( SessionInterface->GetNamedSession(SESSION_NAME) ) // 이미 세션에 접속되어 있는지 확인
	{
		UE_LOG(LogTemp , Warning , TEXT("Already connected to a session. Cannot join again."));
		return; // 이미 접속된 세션이 있다면 다시 접속하지 않도록 방지
	}

	if ( ServerWidget )
	{
		ServerWidget->RemoveUI(); //서버 UI제거
	}

	SessionInterface->JoinSession(0 , SESSION_NAME , SessionSearch->SearchResults[Index]);
}
//세션생성함수
void UK_GameInstance::CreateSession()
{
}
//서버리스트 업데이트함수
void UK_GameInstance::RefreshServerList()
{
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

// 3) Travel 관련 함수 ------------------------------------------------------------------------------------
void UK_GameInstance::TravelMainLobbyMap(bool bKeepCurrentSound)
{
	// K_PlayerController를 가져온다,
	AK_PlayerController* pc = Cast<AK_PlayerController>(GetFirstLocalPlayerController());
	if ( pc && pc->IsLocalController() ) // 컨트롤러가 있으면,
	{
		if ( false == bKeepCurrentSound ) // false 인자인 경우 기존 사운드를 유지하지 않으면서 이동
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
	if ( LobbySound )
	{
		if ( CurrentPlayingSound )
		{
			CurrentPlayingSound->Stop();
		}
		CurrentPlayingSound = UGameplayStatics::SpawnSound2D(this , LobbySound , 1.0f , 1.0f , 0.0f , nullptr , true , false);
		UE_LOG(LogTemp , Warning , TEXT("Started playing lobby sound"));
	}
	else
	{
		UE_LOG(LogTemp , Error , TEXT("LobbySound is not set"));
	}
}
// 사운드 재생 중지 함수
void UK_GameInstance::StopCurrentSound()
{
	if ( CurrentPlayingSound && CurrentPlayingSound->IsPlaying() )
	{
		CurrentPlayingSound->Stop();
		UE_LOG(LogTemp , Warning , TEXT("Stopped current sound"));
	}
}

// 현재 사운드 유지 함수(로그인시)
void UK_GameInstance::ContinueCurrentSound()
{
	// 현재 재생 중인 사운드가 있다면 계속 재생
	if ( CurrentPlayingSound && !CurrentPlayingSound->IsPlaying() )
	{
		CurrentPlayingSound->Play();
	}
	else if ( !CurrentPlayingSound )
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
void UK_GameInstance::MyServerRequest(const UWorld* world , EEventType type , const FString& sendJsonData , UK_GameInstance* gameInstance)
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
	GEngine->AddOnScreenDebugMessage(-1 , 10.f , FColor::Green , FString::Printf(TEXT("%s 요청 시작") , *UEnum::GetValueAsString(type)));

	// 요청 타입 에 따라 다른 요청 실행
	switch ( type )
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
void UK_GameInstance::RequestData(FServerResponseDel resDel , const FString& jsonData , const FString& url , bool useDefaultURL , EReqType type)
{
	// 3-1) 통신 모듈 생성
	FHttpModule* http = &FHttpModule::Get();
	// 3-2) 요청 정보 설정
	TSharedRef<IHttpRequest , ESPMode::ThreadSafe> httpReq = http->CreateRequest();
	// 3-3) 응답 함수 연결( 람다로 연결해서 정해놓은 반응 딜리게이트로 연결되도록 )
	httpReq->OnProcessRequestComplete().BindLambda([resDel , this](FHttpRequestPtr req , FHttpResponsePtr res , bool isSuccess)
	{
		// 성공 여부 확인
		if ( isSuccess )
		{
			// 로그인 시에만 작동, 인증 값 저장해두기
			authorValue = res->GetHeader(TEXT("Authorization"));
			// GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("인증 값 : %s"), *authorValue));
			// json string 가져오기
			FString resStr = res->GetContentAsString();
			// 딜리게이트에 바인드된 반응 함수 실행 
			resDel.ExecuteIfBound(resStr , isSuccess);
		}
		else {
			GEngine->AddOnScreenDebugMessage(-1 , 3.f , FColor::Green , TEXT("요청 실패"));
		}
	});
	// 3-5) 기본 url 사용 여부에 따라 url 설정
	FString resultURL = useDefaultURL ? defaultURL + url : url;
	httpReq->SetURL(resultURL);
	// 3-6) 전송 타입을 설정 POST && 보낼 jsondata가 있다면 넣기
	if ( type == EReqType::POST && !jsonData.IsEmpty() )
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

	GEngine->AddOnScreenDebugMessage(-1 , 31.f , FColor::Yellow , FString::Printf(TEXT("%s") , *resData.ResponseToString()));

	// 바인드된 함수 실행(BroadCast)
	RegisterResUseDel.ExecuteIfBound(resData);
}

//로그인 요청 콜백 바인딩 함수
void UK_GameInstance::MyResLogin(const FString& jsonData , bool isSuccess)
{
	// json 데이터 -> 구조체로 변경
	FLoginResponse resData;
	FJsonObjectConverter::JsonObjectStringToUStruct(jsonData , &resData , 0 , 0);

	GEngine->AddOnScreenDebugMessage(-1 , 31.f , FColor::Yellow , FString::Printf(TEXT("%s") , *resData.ResponseToString()));

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


