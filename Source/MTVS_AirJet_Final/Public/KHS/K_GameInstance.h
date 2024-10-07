// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "../../../../Plugins/Online/OnlineSubsystem/Source/Public/Interfaces/OnlineSessionInterface.h"
#include "KHS/K_SessionInterface.h"
#include "UObject/Interface.h"
#include "K_GameInstance.generated.h"

/**
 * 
 */
UCLASS()
class MTVS_AIRJET_FINAL_API UK_GameInstance : public UGameInstance, public IK_SessionInterface
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
	//UFUNCTION(BlueprintCallable, Category = "Create Widget")
	//void CreateLoginWidget(); // 로그인 UI를 생성하는 함수

	//UFUNCTION(BlueprintCallable, Category = "Create Widget")
	//void CreateServerWidget(); // 시작화면 UI를 생성하는 함수

	//UFUNCTION(BlueprintCallable, Category = "Create Widget")
	//void CreateInGameWidget(); // 인게임 UI를 생성하는 함수

	// 3) Travel 관련 함수 ------------------------------------------------------------------------------------
	UFUNCTION(BlueprintCallable, Category = "Load Widget Map")
	void LoadServerWidgetMap(bool bKeepCurrentSound); // ServerWidget UI가 있는 맵으로 로드시키는 함수 (UI를 레벨에 Attach 해놓았음.)


};
