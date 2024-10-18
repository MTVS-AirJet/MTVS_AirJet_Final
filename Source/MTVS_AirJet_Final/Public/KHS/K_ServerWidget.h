// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KHS/K_WidgetBase.h"
#include "KHS/K_JsonParseLib.h"
#include "K_ServerWidget.generated.h"



USTRUCT()
struct FServerData
{
	GENERATED_BODY()

	int sessionIdx; //방인덱스
	FString sessionName; //방이름
	uint16 curPlayers; //현재 플레이어수
	uint16 maxPlayers; //방정원
	FString hostUserName; //방장이름
};

/**
 * 
 */
UCLASS()
class MTVS_AIRJET_FINAL_API UK_ServerWidget : public UK_WidgetBase
{
	GENERATED_BODY()
	

public:
	//===============================================================
	// 생성자, 초기화
	//===============================================================

	UK_ServerWidget(const FObjectInitializer& ObjectInitialize);

	virtual void NativeConstruct() override;

	virtual bool Initialize();

	//===============================================================
	// TSubClass & Class Instance
	//===============================================================
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> ServerListFactory; // ServerList 공장
	class UK_ServerList* ServerList; // ServerLIst인스턴스

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> PlayerListFactory; // PlayerList 공장
	class UK_PlayerList* PlayerList; // PlayerLIst인스턴스


	//===============================================================
	// Widget Binding
	//===============================================================
#pragma region Widget Binding
	
	// 위젯 스위치, Animation ==================================================
	UPROPERTY(meta = (BindWidget))
	class UWidgetSwitcher* MenuSwitcher; // 각 위젯으로 전환시킬 수 있는 Menu Switcher

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* ShowTransitionAnim; 

	UPROPERTY(meta = (BindWidgetAnim) , Transient)
	class UWidgetAnimation* ShowServerMenuAnim; 
	
	UPROPERTY(meta = (BindWidgetAnim) , Transient)
	class UWidgetAnimation* HideServerMenuAnim;

	UPROPERTY(meta = (BindWidgetAnim) , Transient)
	class UWidgetAnimation* ShowHostMenuAnim;
	
	UPROPERTY(meta = (BindWidgetAnim) , Transient)
	class UWidgetAnimation* HideHostMenuAnim;

	UPROPERTY(meta = (BindWidgetAnim) , Transient)
	class UWidgetAnimation* ShowReadyMenuAnim;
	
	UPROPERTY(meta = (BindWidgetAnim) , Transient)
	class UWidgetAnimation* HideReadyMenuAnim;

	// Server Menu ==================================================

	UPROPERTY(meta = (BindWidget))
	class UWidget* ServerMenu; // 서버 메뉴 Widget UI

	UPROPERTY(meta = (BindWidget))
	class UPanelWidget* ServerMenu_ServerList; // ServerList 출력 ScrollBox

	UPROPERTY(meta = (BindWidget))
	class UButton* ServerMenu_btn_Create; // 방 생성 버튼 

	UPROPERTY(meta = (BindWidget))
	class UButton* ServerMenu_btn_Join; // 방 참여 버튼 

	UPROPERTY(meta = (BindWidget))
	class UButton* ServerMenu_btn_Home; // 로비 돌아가기 버튼 
	
	UPROPERTY(meta = (BindWidget))
	class UButton* ServerMenu_btn_Reset; // ServerList Update 버튼 

	UPROPERTY(meta = (BindWidget))
	class UImage* ServerMenu_img_Map; // 해당 세션 맵 썸네일

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ServerMenu_txt_Producer; // 해당 세션의 맵제작자 이름

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ServerMenu_txt_MapName; // 해당 세션의 맵 이름

	// Host Menu ====================================================
	UPROPERTY(meta = (BindWidget))
	class UWidget* HostMenu; // Host 메뉴 Widget UI

	UPROPERTY(meta = (BindWidget))
	class UButton* HostMenu_btn_Create; // 방 생성 버튼 

	UPROPERTY(meta = (BindWidget))
	class UButton* HostMenu_btn_LoadMap; // 맵 불러오기 버튼 

	UPROPERTY(meta = (BindWidget))
	class UButton* HostMenu_btn_Cancel; // 로비로 돌아가기 버튼
	
	UPROPERTY(meta = (BindWidget))
	class UButton* HostMenu_btn_WebQuit; // 로비로 돌아가기 버튼
	
	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* HostMenu_txt_RoomName; // 방이름 입력 텍스트

	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* HostMenu_txt_RoomPW; // 방비밀번호 입력 텍스트

	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* HostMenu_txt_LoadedMapName; // 불러온 맵의 맵이름

	UPROPERTY(meta = (BindWidget))
	class UWebBrowser* HostMenu_web_Popup; // 맵불러오기URL팝업
	

	// Ready Menu ===================================================

	UPROPERTY(meta = (BindWidget))
	class UWidget* ReadyMenu; // Ready 메뉴 Widget UI

	UPROPERTY(meta = (BindWidget))
	class UPanelWidget* ReadyMenu_PlayerList; // PlayerList 출력 ScrollBox

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ReadyMenu_txt_RoomName; // 생성된 방 이름

	UPROPERTY(meta = (BindWidget))
	class UButton* ReadyMenu_btn_Start; // 게임시작하기 버튼(게임맵 트래블)

	UPROPERTY(meta = (BindWidget))
	class UButton* ReadyMenu_btn_Home; // 로비로 돌아가기 버튼

	UPROPERTY(meta = (BindWidget))
	class UImage* ReadyMenu_img_Map; // 해당 세션 맵 썸네일

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ReadyMenu_txt_Producer; // 해당 세션의 맵제작자 이름

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ReadyMenu_txt_MapName; // 해당 세션의 맵 이름

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ReadyMenu_txt_Latitude; // 맵 정보(위도)

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ReadyMenu_txt_Longitude; // 맵 정보(경도)

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ReadyMenu_txt_CommandList; // 맵 미션목록

#pragma endregion
	
	//===============================================================
	// 클래스 및 인스턴스
	//===============================================================
	TOptional <int> SelectedIndex; // 서버의 인덱스를 참조 선언

	class UK_GameInstance* GameInstance; // GameInstance 참조 선언

	//===============================================================
	// 전역 변수
	//===============================================================

	FString CreatedMapData;


	//===============================================================
	// Functions
	//===============================================================

#pragma region Functions
	
	// Common =======================================================

	UFUNCTION(BlueprintCallable)
	void OpenServerMenuFromHost(); // Server메뉴로 돌아가는 함수

	UFUNCTION(BlueprintCallable)
	void OpenServerMenuFromReady(); // Server메뉴로 돌아가는 함수

	UFUNCTION(BlueprintCallable)
	void OpenLobbyLevel(); // Lobby 맵으로 돌아가는 함수

	UFUNCTION(BlueprintCallable)
	void OpenHostMenuFromServer(); // Host 메뉴로 접속하는 함수

	// Server Menu ==================================================

	void SetServerList(TArray<FServerData> sessionName); // 상단에 선언된 FServerData 구조체를 바탕으로 Session 목록을 설정하는 함수

	UFUNCTION(BlueprintCallable)
	void ServerListUpdateChildren(); //ServerList업데이트

	void SelecetIndex(int Index); // 서버의 인덱스를 선택하는 함수


	UFUNCTION(BlueprintCallable)
	void ReqSessionInfo(const FMapInfoRequest& mapName); //선택한 인덱스의 세션정보에서 MapData정보를 얻어오는 함수

	UFUNCTION(BlueprintCallable)
	void OnResSessionInfo(const FMapInfoResponse& resData); //FMapInfoResponse구조체로 맵정보 란에 정보를 업데이트 하는 함수

	UFUNCTION(BlueprintCallable)
	void JoinRoom(); // 생성된 Session에 접속하는 함수

	
	// Host Menu ====================================================

	UFUNCTION(BlueprintCallable)
	void OpenCreaterWeb(); // 크리에이터툴 웹서비스로 접속하는 함수

	UFUNCTION(BlueprintCallable)
	void QuitCreaterWeb(); // 웹브라우저 위젯 종료 함수

	UFUNCTION(BlueprintCallable)
	void ReqMapInfo(); //mapName입력내용 기준으로 서버에 요청하는 함수

	UFUNCTION(BlueprintCallable)
	void ResMapInfo(const FMapInfoResponse& resData); //서버요청 콜백 바인딩_해당 맵정보를 받아와 HOST MENU와 SESSION MENU에 세팅하는 함수

	UFUNCTION(BlueprintCallable)
	void CreateRoom(); // (현재) Interface에서 Host 함수를 호출하는 함수 (Origin) ReadyMenu로 정보를 가진채 넘어가기.


	// Ready Menu ===================================================

	UFUNCTION(BlueprintCallable)
	void SetPlayerList(); // PlayerList ScrollBox에 플레이어 정보를 Set하는 함수

	UFUNCTION(BlueprintCallable)
	void PlayerListUpdateChildren(); // PlayerList 3초마다 업데이트

	UFUNCTION(BlueprintCallable)
	void LoadGameMap(); //게임시작 버튼 바인딩 함수(게임맵 트래블)
	


	

	

	


#pragma endregion

};
