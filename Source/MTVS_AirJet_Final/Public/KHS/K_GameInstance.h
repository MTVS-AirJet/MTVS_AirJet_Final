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




};
