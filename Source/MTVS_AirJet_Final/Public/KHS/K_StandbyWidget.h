// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KHS/K_WidgetBase.h"
#include "K_StandbyWidget.generated.h"

/**
 * 
 */
UCLASS()
class MTVS_AIRJET_FINAL_API UK_StandbyWidget : public UK_WidgetBase
{
	GENERATED_BODY()
	
public:
	//===============================================================
	// 생성자, 초기화
	//===============================================================

	UK_StandbyWidget(const FObjectInitializer& ObjectInitialize);

	virtual void NativeConstruct() override;

	virtual bool Initialize();

	virtual void SetUI() override;

	virtual void RemoveUI() override;

	//===============================================================
	// TSubClass & Class Instance
	//===============================================================

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> PlayerListFactory; // PlayerList 공장

	class UK_PlayerList* PlayerList; // PlayerLIst인스턴스

	class UK_GameInstance* GameInstance; // GameInstance 참조 선언

	class AK_PlayerController* PlayerController; //PlayerControler 참조 선언

	class AK_GameState* KGameState; //GameState 참조 선언

	//===============================================================
	// Widget Binding
	//===============================================================
#pragma region Widget Binding
	
	// 위젯 스위치, Animation ==================================================
	UPROPERTY(meta = (BindWidget))
	class UWidgetSwitcher* MenuSwitcher; // 각 위젯으로 전환시킬 수 있는 Menu Switcher

	UPROPERTY(meta = (BindWidgetAnim) , Transient)
	class UWidgetAnimation* ShowStandbyMenuAnim;
	
	UPROPERTY(meta = (BindWidgetAnim) , Transient)
	class UWidgetAnimation* HideStandbyMenuAnim;

	// Standby Menu ===================================================

	UPROPERTY(meta = (BindWidget))
	class UWidget* StandbyMenu; // Ready 메뉴 Widget UI

	UPROPERTY(meta = (BindWidget))
	class UPanelWidget* StandbyMenu_PlayerList; // PlayerList 출력 ScrollBox

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* StandbyMenu_txt_RoomName; // 생성된 방 이름

	UPROPERTY(meta = (BindWidget))
	class UButton* StandbyMenu_btn_Start; // 게임시작하기 버튼(게임맵 트래블)

	UPROPERTY(meta = (BindWidget))
	class UButton* StandbyMenu_btn_Home; // 로비로 돌아가기 버튼

	UPROPERTY(meta = (BindWidget))
	class UImage* StandbyMenu_img_Map; // 해당 세션 맵 썸네일

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* StandbyMenu_txt_Producer; // 해당 세션의 맵제작자 이름

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* StandbyMenu_txt_MapName; // 해당 세션의 맵 이름

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* StandbyMenu_txt_Latitude; // 맵 정보(위도)

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* StandbyMenu_txt_Longitude; // 맵 정보(경도)

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* StandbyMenu_txt_CommandList; // 맵 미션목록

#pragma endregion

	//===============================================================
	// Variables
	//===============================================================

	FTimerHandle PlayerListUpdateTimer; //플레이어 리스트 업데이트 타이머

	//===============================================================
	// Functions
	//===============================================================

#pragma region Functions

	// Ready Menu ===================================================
	UFUNCTION(BlueprintCallable)
	void SetPlayerList(); // PlayerList ScrollBox에 플레이어 정보를 Set하는 함수

	UFUNCTION(BlueprintCallable)
	void PlayerListUpdateChildren(); // PlayerList 2초마다 업데이트

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRequestPlayerListUpdate();  // 서버에서 PlayerList 업데이트 요청

	UFUNCTION(Client, Reliable)
	void ClientUpdatePlayerList(const TArray<FString>& PlayerNames);  // 모든 클라이언트에서 PlayerList 업데이트


	UFUNCTION(BlueprintCallable)
	void StartMission(); //게임시작 버튼 바인딩 함수

	UFUNCTION(BlueprintCallable)
	void OpenLobbyLevel(); //로비레벨로 돌아가는 함수

    UFUNCTION(BlueprintCallable)
    void InitializeMissionData(); // GameInstance의 MissionData로 위젯 설정하는 함수

#pragma endregion

};
