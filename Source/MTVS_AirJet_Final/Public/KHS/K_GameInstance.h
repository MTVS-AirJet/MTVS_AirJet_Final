// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KHS/K_SessionInterface.h"
#include "JBS/J_GameInstance.h"
#include "Engine/GameInstance.h"
#include "../../../../Plugins/Online/OnlineSubsystem/Source/Public/Interfaces/OnlineSessionInterface.h"
#include "UObject/Interface.h"
#include <JBS/J_Utility.h>
#include <JsonObjectConverter.h>
#include "K_GameInstance.generated.h"

/**
 * 
 */
#pragma region 딜리게이트 추가 영역
DECLARE_DELEGATE_TwoParams(FResponseDelegate, const FString&, bool);

DECLARE_DELEGATE_OneParam(FResSimpleDelegate, const FResSimple&);

// _One~ _NineParams 까지 가능
#pragma endregion

UCLASS()
class MTVS_AIRJET_FINAL_API UK_GameInstance : public UJ_GameInstance, public IK_SessionInterface
{
	GENERATED_BODY()
	
public: 

	




	// 1) 세션 관련 함수 --------------------------------------------------------------------------------------
	UFUNCTION(Exec) // Exec: 콘솔창에 입력할 수 있도록 만든다.
	void Host(FString ServerName) override; // 서버 열기 함수
	
	UFUNCTION(Exec)
	void Join(uint32 Index); // 서버 접속 함수

	UFUNCTION()
	void CreateSession(); // 세션을 만드는 함수

	UFUNCTION()
	void RefreshServerList(); // 서버목록을 찾는 함수


	// 2) UI 생성 관련 함수 -----------------------------------------------------------------------------------
	UFUNCTION(BlueprintCallable, Category = "Create Widget")
	void CreateLoginWidget(); // 로그인 UI를 생성하는 함수

	//UFUNCTION(BlueprintCallable, Category = "Create Widget")
	//void CreateServerWidget(); // 시작화면 UI를 생성하는 함수

	//UFUNCTION(BlueprintCallable, Category = "Create Widget")
	//void CreateInGameWidget(); // 인게임 UI를 생성하는 함수

	// 3) Travel 관련 함수 ------------------------------------------------------------------------------------
	UFUNCTION(BlueprintCallable, Category = "Load Widget Map")
	void LoadServerWidgetMap(bool bKeepCurrentSound); // ServerWidget UI가 있는 맵으로 로드시키는 함수 (UI를 레벨에 Attach 해놓았음.)

	// 5) 사운드 관련 함수 --------------------------------------------------------------------------------------
	void PlayLobbySound(); // 로비 사운드 재생 함수
	//UFUNCTION()
	//void PlayStageSound(); // 시뮬레이션 스테이지 사운드 재생 함수
	//void StopCurrentSound(); // 현재 사운드 재생 중지 함수
	void ContinueCurrentSound(); // 현재 사운드 유지 함수


	//===============================================================

	// 1) UI 관련 참조 ------------------------------------------------------------------------------------------
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UK_LoginRegisterWidget> LoginWidgetFactory; // ServerWidget(UI) 공장
	class UK_LoginRegisterWidget* LoginWidget; // ServerWidget(UI) 참조 선언
	
	// 2) 사운드 관련 참조 ----------------------------------------------------------------------------------------------
	UPROPERTY(EditAnywhere, Category = "Sound")
	class USoundWave* LobbySound; // 로비 사운드

	UPROPERTY(EditAnywhere, Category = "Sound")
	class USoundWave* StageSound; // 스테이지(맵) 사운드

	UPROPERTY()
	class UAudioComponent* CurrentPlayingSound; // 현재 재생 중인 사운드를 추적하기 위한 변수



#pragma region BackEnd Coordination
	//=============================================================
	// 백엔드 파트 협업
	//=============================================================
	//유저토큰 저장용도
	UPROPERTY()
    FString AuthToken;

    UFUNCTION(BlueprintCallable, Category = "Auth")
    void SetAuthToken(const FString& Token) { AuthToken = Token; }

    UFUNCTION(BlueprintCallable, Category = "Auth")
    FString GetAuthToken() const { return AuthToken; }
	//유저ID 저장용도
	void SetUserId(const FString& NewUserId);
	FString GetUserId() const;
	FString UserId;
	//=============================================================
#pragma endregion








#pragma region 여기부터 JBS 작성 단
protected:
	// 기본 ip
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
	FString defaultURL = TEXT("http://125.132.216.190:7757/api/");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
	FString authorValue = TEXT("");

	// 전송 타입 맵
	TMap<ERequestType, FString> reqTypeMap = {
		{ERequestType::GET, TEXT("GET")}
		,{ERequestType::POST, TEXT("POST")}
	};	

	// json 데이터 변환 시킬 res 함수 연결용 딜리게이트
	// 통신 단위 하나당 한 개 씩 추가
	FResponseDelegate tempDel;

	FResponseDelegate signupDel;

	FResponseDelegate loginDel;

	FResponseDelegate tempLoginAuthDel;

	// 미션
	// 자신 플레이어 역할
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values", BlueprintGetter=GetPlayerRole, BlueprintSetter=SetPlayerRole)
	EPlayerRole playerRole;
		public:
	__declspec(property(get = GetPlayerRole, put = SetPlayerRole)) EPlayerRole PLAYER_ROLE;
	UFUNCTION(BlueprintGetter)
	EPlayerRole GetPlayerRole()
	{
		return playerRole;
	}
	UFUNCTION(BlueprintSetter)
	void SetPlayerRole(EPlayerRole value)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString::Printf(TEXT("현재 플레이어 역할 : %s"), *UEnum::GetValueAsString(value)));
		playerRole = value;
	}
		protected:

	// 미션 플레이어 역할, 캐릭터 프리팹 맵
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Classes")
	TMap<EPlayerRole, TSubclassOf<class APawn>> playerPrefabMap;

public:
#pragma region 사용 함수 연결용 딜리게이트 단
	// res 구조체 데이터 사용할 함수 연결용 딜리게이트
	FResSimpleDelegate tempLoginAuthUseDel;

#pragma endregion

protected:
	// 서버에 요청
	// 구조체 데이터 있음 == 대부분 post 중 일부
	template<typename InStructType>
	void ReqData(FResponseDelegate resDel, const InStructType& structData, const FString& url = "", bool useDefaultURL = true, ERequestType type = ERequestType::POST)
	{
		// 구조체 -> json 화
		FString jsonData;
		FJsonObjectConverter::UStructToJsonObjectString(structData, jsonData, 0, 0);

		ReqData(resDel, jsonData, url, useDefaultURL, type);
	}

	// 구조체 데이터 없음 == get, post 일부
	// json 데이터 가지고 서버 요청
	void ReqData(FResponseDelegate resDel, const FString& jsonData = TEXT(""),  const FString& url = "", bool useDefaultURL = true, ERequestType type = ERequestType::POST);

	// solved 테스트용 데이터 주고 받기
	UFUNCTION(BlueprintCallable)
	void ReqTempCallback();

	UFUNCTION()
	void ResTempCallback(const FString& jsonData, bool isSuccess);

	UFUNCTION()
	void ResSignup(const FString &jsonData, bool isSuccess);

	UFUNCTION()
	void ResLogin(const FString &jsonData, bool isSuccess);

	UFUNCTION()
	void ResLoginAuth(const FString &jsonData, bool isSuccess);

public:
	// 통신 타입, 데이터 받아서 서버에 요청 시작
	template<typename InStructType>
	void RequestToServer(EJsonType type, const InStructType& structData)
	{
		// 구조체 데이터 json 변환
		FString jsonData;
		FJsonObjectConverter::UStructToJsonObjectString(structData, jsonData, 0, 0);

		// req2server
		RequestToServer(type, jsonData);
	}

	void RequestToServer(EJsonType type, const FString &sendJsonData = TEXT(""));

	// 자신 플레이어 역할에 맞는 프리팹 주기
	TSubclassOf<APawn> GetMissionPlayerPrefab();

#pragma endregion



};
