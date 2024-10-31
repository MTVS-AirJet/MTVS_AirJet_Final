// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KHS/K_SessionInterface.h"
#include "K_JsonParseLib.h"
#include "JBS/J_GameInstance.h"
#include <JBS/J_Utility.h>
#include "Engine/GameInstance.h"
#include "Engine/StreamableManager.h"
#include "../../../../Plugins/Online/OnlineSubsystem/Source/Public/Interfaces/OnlineSessionInterface.h"
#include "UObject/Interface.h"
#include <JsonObjectConverter.h>
#include <map>

#include "K_GameInstance.generated.h"

/**
 * 
 */

// Deligate 선언 (델리게이트 - Game Instance용)
DECLARE_DELEGATE_TwoParams(FServerResponseDel , const FString& , bool);

// Deligate 선언 (델리게이트 - JSON 사용 클래스용, JsonParseLib구조체 사용)
DECLARE_DELEGATE_OneParam(FRegisterResponseDel , const FRegisterResponse&); //회원가입 이벤트
DECLARE_DELEGATE_OneParam(FLoginResponseDel , const FLoginResponse&); //로그인 이벤트
DECLARE_DELEGATE_OneParam(FMapInfoResponseDel , const FMapInfoResponse&); //맵정보요청 이벤트


UCLASS()
class MTVS_AIRJET_FINAL_API UK_GameInstance : public UJ_GameInstance , public IK_SessionInterface
{
	GENERATED_BODY()

private:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps);

public:
	//===============================================================
	// 전역 인스턴스, 클래스 참조
	//===============================================================
	IOnlineSessionPtr SessionInterface; // 세션 인터페이스를 전역인수로 선언

	TSharedPtr<class FOnlineSessionSearch> SessionSearch; // 온라인 세션 검색을 할 수 있는 클래스 인스턴스 선언

	FString DesiredServerName; // Host 시 서버 이름을 지정하여 설정하기 위한 변수

	FString DesiredServerData;

	class AK_GameState* KGameState;
	class AJ_MissionGameState* JGameState;

	std::map<int32 , FString> ServerDataList;

	FMissionDataRes MissionData; //미션관리자가 사용할 미션데이터 구조체

	UPROPERTY(BlueprintReadOnly , Category = "Session" , ReplicatedUsing=OnConnectedPlayerNames)
	TArray<FString> ConnectedPlayerNames; // 세션에 접속한 플레이어 이름 배열

	UFUNCTION()
	void OnConnectedPlayerNames();
	//===============================================================
	// Functions
	//===============================================================

	// 생성자 & 초기화 함수==========================================

	UK_GameInstance(const FObjectInitializer& ObjectInitializer); // 생성자 초기화

	virtual void Init() override;

	virtual void OnStart() override;

	// 델리게이트 바인딩 함수 구간 ====================================

	void OnCreateSessionComplete(FName SessionName , bool Success); // 세션 생성 완료 시 호출될 바인딩 함수
	void OnDestroySessionComplete(FName SessionName , bool Success); // 세션 파괴 완료 시 호출될 바인딩 함수
	void OnFindSessionComplete(bool Success); // 세션 찾기 완료 시 호출될 바인딩 함수.
	void OnJoinSessionComplete(FName SessionName , EOnJoinSessionCompleteResult::Type Result); // 세션 Join 완료시 호출된 바인딩 함수
	void OnNetworkFailure(UWorld* World , UNetDriver* NetDriver , ENetworkFailure::Type FailureType ,
	                      const FString& ErrorString); //네트워크서버 찾기 실패시 호출

	// 세션 관리 함수 구간 ===========================================

#pragma  region Session Management Functions
	// 1) 세션 관련 함수 --------------------------------------------------------------------------------------
	UFUNCTION(Exec) // Exec: 콘솔창에 입력할 수 있도록 만든다.
	void Host(FString ServerName , const FString& MapDataStruct) override; // 서버 열기 함수

	UFUNCTION(Exec)
	void Join(uint32 Index); // 서버 접속 함수

	UFUNCTION()
	void CreateSession(); // 세션을 만드는 함수

	UFUNCTION()
	void RefreshServerList(); // 서버목록을 찾는 함수

	UFUNCTION()
	void OnMapPreloadComplete(); // Map 비동기 load 함수

	// 2) UI 생성 관련 함수 -----------------------------------------------------------------------------------
	UFUNCTION(BlueprintCallable , Category = "Create Widget")
	void CreateLoginWidget(); // 로그인 UI를 생성하는 함수

	UFUNCTION(BlueprintCallable , Category = "Create Widget")
	void CreateServerWidget(); // Server UI를 생성하는 함수

	UFUNCTION(BlueprintCallable , Category = "Create Widget")
	void CreateLoadingWidget(); // Loading UI를 생성하는 함수

	UFUNCTION(BlueprintCallable , Category = "Create Widget")
	void CreateStandbyWidget(); // Standby UI를 생성하는 함수

	UFUNCTION(BlueprintCallable , Category = "Create Widget")
	void InitializeMission(const FMissionDataRes& newMD); //미션데이터를 GameState를 통해 StandbyUI에 업데이트해주는 함수

	//UFUNCTION(BlueprintCallable, Category = "Create Widget")
	//void CreateInGameWidget(); // 인게임 UI를 생성하는 함수

	// 3) Travel 관련 함수 ------------------------------------------------------------------------------------
	UFUNCTION(BlueprintCallable , Category = "Load Lobby Map")
	void TravelMainLobbyMap(bool bKeepCurrentSound); // Lobby 맵으로 트래블시키는 함수

	// 4) 사운드 관련 함수 --------------------------------------------------------------------------------------
	void PlayLobbySound(); // 로비 사운드 재생 함수
	UFUNCTION()
	void PlayStageSound(); // 시뮬레이션 스테이지 사운드 재생 함수
	void StopCurrentSound(); // 현재 사운드 재생 중지 함수
	void ContinueCurrentSound(); // 현재 사운드 유지 함수

#pragma endregion


	//===============================================================
	// Variables
	//===============================================================
#pragma region Session Management Variables

	// 1) 세션 관련 참조 ------------------------------------------------------------------------------------------
	FStreamableManager StreamableManager; //Map 비동기함수 관련


	// 2) UI 관련 참조 ------------------------------------------------------------------------------------------
	UPROPERTY(EditAnywhere , Category = "UI")
	TSubclassOf<class UK_LoginRegisterWidget> LoginWidgetFactory; // LoginWidget(UI) 공장
	class UK_LoginRegisterWidget* LoginWidget; // LoginWidget(UI) 참조 선언

	UPROPERTY(EditAnywhere , Category = "UI")
	TSubclassOf<class UK_ServerWidget> ServerWidgetFactory; // ServerWidget(UI) 공장
	class UK_ServerWidget* ServerWidget; // ServerWidget(UI) 참조 선언

	UPROPERTY(EditAnywhere , Category = "UI")
	TSubclassOf<class UK_LoadingWidget> LoadingWidgetFactory; // LoadingWidget(UI) 공장
	class UK_LoadingWidget* LoadingWidget; // LoadingWidget(UI) 참조 선언

	UPROPERTY(EditAnywhere , Category = "UI")
	TSubclassOf<class UK_StandbyWidget> StandbyWidgetFactory; // StandbyWidget(UI) 공장
	class UK_StandbyWidget* StandbyWidget; // StandbyWidget(UI) 참조 선언


	//UPROPERTY(EditAnywhere, Category = "UI")
	//TSubclassOf<class UK_IngameWidget> IngameWidgetFactory; // IngameWidget(UI) 공장
	//class UK_IngameWidget* IngmaeWidget; // IngameWidget(UI) 참조 선언

	// 1) 사운드 관련 참조 ----------------------------------------------------------------------------------------------
	UPROPERTY(EditAnywhere , Category = "Sound")
	class USoundWave* LobbySound; // 로비 사운드

	UPROPERTY(EditAnywhere , Category = "Sound")
	class USoundWave* StageSound; // 스테이지(맵) 사운드

	UPROPERTY()
	class UAudioComponent* CurrentPlayingSound; // 현재 재생 중인 사운드를 추적하기 위한 변수


#pragma endregion

	//=============================================================
	// 백엔드 파트 협업
	//=============================================================

#pragma region Server Part Coordination

	//유저토큰 저장용도
	UPROPERTY()
	FString AuthToken;

	UFUNCTION(BlueprintCallable , Category = "Auth")
	void SetAuthToken(const FString& Token) { AuthToken = Token; }

	UFUNCTION(BlueprintCallable , Category = "Auth")
	FString GetAuthToken() const { return AuthToken; }

	//유저ID 저장용도
	void SetUserId(const FString& NewUserId);
	FString GetUserId() const;
	FString UserId;
	//=============================================================

	// 전송 타입 맵
	TMap<EReqType , FString> ReqTypeMap = {
		{EReqType::GET , TEXT("GET")} , {EReqType::POST , TEXT("POST")}
	};


	// 1) 서버 요청 탬플릿 ---------------------------------------------
#pragma region Server Request STRUCT / TEMPLATE

	// 1. [탬플릿] 웹통신요청 : GetWorld(), 요청 타입, 구조체 데이터, 게임 인스턴스(생략가능)
	template <typename DataStructType>
	static void MyServerRequest(const UWorld* world
	                            , EEventType type
	                            , const DataStructType& structData
	                            , UK_GameInstance* gameInstance = nullptr)
	{
		// 구조체 데이터를 json string 데이터로 변환(내장함수 사용)
		FString jsonData;
		FJsonObjectConverter::UStructToJsonObjectString(structData , jsonData , 0 , 0);

		// 아래 함수로 요청 시작
		MyServerRequest(world , type , jsonData , gameInstance);
	}

	//// 2. [탬플릿] 통신 타입, 데이터 받아서 서버에 요청 시작 : 요청타입, JsonString데이터(생략가능)
	//template<typename DataStructType>
	//void RequestToServerByType(EEventType type , const DataStructType& structData)
	//{
	//	// 구조체 데이터 json 변환
	//	FString jsonData;
	//	FJsonObjectConverter::UStructToJsonObjectString(structData , jsonData , 0 , 0);

	//	// 서버에 요청 시작
	//	RequestToServerByType(type , jsonData);
	//}

	//// 3. [탬플릿] 서버에 요청 : 반응 딜리게이트, 구조체 데이터, url, 기본 url 사용 여부, 전송 타입(POST or GET)
	//template<typename DataStructType>
	//void RequestData(FServerResponseDel resDel , const DataStructType& structData , const FString& url = "" , bool useDefaultURL = true , EReqType type = EReqType::POST)
	//{
	//	// 구조체 -> json 화
	//	FString jsonData;
	//	FJsonObjectConverter::UStructToJsonObjectString(structData , jsonData , 0 , 0);

	//	RequestData(resDel , jsonData , url , useDefaultURL , type);
	//}

#pragma endregion

	// 2) 서버 요청 사용함수 -------------------------------------------
#pragma region Server Request Functions

	// 1. 웹 통신 요청 함수 : getworld(), 요청 타입, json string 데이터(생략가능), 게임 인스턴스(생략가능)
	static void MyServerRequest(const UWorld* world , EEventType type , const FString& sendJsonData = TEXT("") ,
	                            UK_GameInstance* gameInstance = nullptr);

	// 2. 요청 타입에 따라 다른 요청 실행 : 요청 타입, json string 데이터(생략 가능)
	virtual void RequestToServerByType(EEventType type , const FString& sendJsonData = TEXT(""));

	// 3. 서버에 요청 : 반응 딜리게이트, json 데이터, url, 기본 url 사용 여부, 전송 타입(POST or GET)
	void RequestData(FServerResponseDel resDel , const FString& jsonData = TEXT("") , const FString& url = "" ,
	                 bool useDefaultURL = true , EReqType type = EReqType::POST);

#pragma endregion
	// 3) 요청콜백 델리게이트 변수 -------------------------------------
#pragma region Response CallBack Delegate Variables

public:
	// 서버 콜백시 요청콜백함수 연결용 Delegate(이벤트별)

	//GameInstance 클래스 사용 용도(서버처리)
	FServerResponseDel RegisterDel; //회원가입 Delegate

	FServerResponseDel LoginDel; //로그인 Delegate

	FServerResponseDel MapInfoDel; //맵정보 Delegate

	//JSON이벤트가 발생하는 타클래스 사용 용도(각 클래스별 처리)
	FRegisterResponseDel RegisterResUseDel; //회원가입 Delegate

	FLoginResponseDel LoginResUseDel; //로그인 Delegate

	FMapInfoResponseDel MapInfoResUseDel; //맵정보 Delegate
	FMapInfoResponseDel MapInfoResUseDel2; //맵정보 Delegate

#pragma endregion
	// 4) 요청콜백 델리게이트 바인딩 함수 ------------------------------
#pragma region Response CallBack Delegate Functions

public:
	//회원가입 요청 콜백 바인딩 함수
	UFUNCTION()
	virtual void MyResRegister(const FString& jsonData , bool isSuccess);
	//로그인 요청 콜백 바인딩 함수
	UFUNCTION()
	virtual void MyResLogin(const FString& jsonData , bool isSuccess);
	//맵정보 요청 콜백 바인딩 함수
	UFUNCTION()
	virtual void MyResMapInfo(const FString& jsonData , bool isSuccess);

#pragma endregion


#pragma endregion

public:
	FString MyName;
	FString JoinRoomName;
	bool bHost;
public:
	UPROPERTY(ReplicatedUsing=OnMyMemberReFresh)
	int32 ReadyMemeberCnt = 0;
	UFUNCTION()
	void OnMyMemberReFresh();
};
