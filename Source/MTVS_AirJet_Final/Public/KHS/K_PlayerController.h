// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "K_PlayerController.generated.h"

/**
 * 
 */
//게임시작 델리게이트 -> CRPC_GameStart에 연결
DECLARE_MULTICAST_DELEGATE(FOnStartGameforViper);
DECLARE_MULTICAST_DELEGATE(FOnStartGameforMission);

UCLASS()
class MTVS_AIRJET_FINAL_API AK_PlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	//델리게이트 변수 선언
	FOnStartGameforMission StartGameDel_Mission; //미션용
	FOnStartGameforViper StartGameDel_Viper; //클라용

	//클래스 인스턴스 참조 선언
	class AK_GameState* KGameState;
	
public:
	virtual void BeginPlay() override;

	virtual void OnPossess(APawn* InPawn) override; // 캐릭터가 Possess될 때 호출되는 함수

    virtual void SetupInputComponent() override; // 입력 구성 요소 설정 함수 선언

	//화면 공유 UI======================================
   UPROPERTY(EditDefaultsOnly)
   TSubclassOf<class UUserWidget> StreamingUIFactory;
   UPROPERTY(BlueprintReadWrite)
   class UK_StreamingUI* StreamingUI;

   //인게임 StandbyUI===================================
   UPROPERTY(EditDefaultsOnly)
   TSubclassOf<class UUserWidget> StandbyUIFactory;
   UPROPERTY(BlueprintReadWrite)
   class UK_StandbyWidget* StandbyUI;

   FString CurrentMapName; //현재 맵네임

    UPROPERTY(EditDefaultsOnly , Category="Inputs")
	class UInputMappingContext* IMC_Viper;

    //클라이언트 UI생성 및 imc 맵핑 RPC함수
    UFUNCTION(Client, Reliable)
    void CRPC_SetIMCnCreateStandbyUI();

	UFUNCTION(Server, Reliable, WithValidation)
	void SRPC_StartGame(); // 전체 클라에 게임시작 선언 델리게이트 바인딩

	UFUNCTION(Client, Reliable)
	void CRPC_StartGame(); // 전체 클라에 게임시작 전달 Client RPC


	// Common Input Widget 관련
#pragma region Common Input, Widget 관련
    // 공통 입력 키 EnhancedInput 관련 --------------------------------------------
    UPROPERTY(EditAnywhere, Category = "EnhancedInput")
    class UInputMappingContext* IMC_Common; // Mapping Context 참조

    UPROPERTY(EditAnywhere, Category = "EnhancedInput")
    class UInputAction* IA_ToggleCommonWidget; // InputAction 참조

    UPROPERTY(EditAnywhere, Category = "EnhancedInput")
    class UInputAction* IA_ToggleMouseCursor; // InputAction 참조

    bool bIsCommonWidgetVisible; // CommonWidget 가시성 상태 변수

    bool bIsMouseCursorShow; // 마우스커서 상태 변수

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<class UK_CommonWidget> CommonWidgetFactory; // CommonWidget(UI) 공장
    class UK_CommonWidget* CommonWidget; // CommonWidget(UI) 참조 선언

    // Common Widget 토글 함수
    void ToggleCommonWidget(const struct FInputActionValue& value);

    // 마우스커서 토글 함수
    void ToggleMouseCursor(const struct FInputActionValue& value);
	
    // 클라이언트를 로비 레벨로 트래블시키는 함수
    UFUNCTION(BlueprintCallable)
    virtual void TravelToLobbyLevel();

#pragma endregion 

};
