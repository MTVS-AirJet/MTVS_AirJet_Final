// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KHS/K_WidgetBase.h"
#include "HttpFwd.h"
#include "K_LoginRegisterWidget.generated.h"

/**
 * 
 */
UCLASS()
class MTVS_AIRJET_FINAL_API UK_LoginRegisterWidget : public UK_WidgetBase
{
	GENERATED_BODY()
	
public:

	//======================================================
	// 초기화, 메뉴스위치, URL,클래스 인스턴스
	//======================================================
	//생성자&초기화
	virtual bool Initialize(); 
	
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UWidgetSwitcher* MenuSwitcher; //UI 스위쳐

	class UK_GameInstance* gi;

	FString SignupURL = "http://52.78.175.85/api/auth/signup";
	//FString SignupURL = "http://43.202.221.239/api/auth/signup";
	//FString SignupURL = "http://125.132.216.190:7757/api/auth/signup";
	//FString LoginURL = "http://125.132.216.190:7757/api/auth/login";
	FString LoginURL = "http://52.78.175.85/api/auth/login";
	//FString LoginURL = "http://43.202.221.239/api/auth/login";

	//======================================================
	// 로그인 Functions, Variables
	//======================================================

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UWidget* LoginMenu; //Login 위젯

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UEditableTextBox* LoginMenu_txt_UserID; //ID입력

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UEditableTextBox* LoginMenu_txt_UserPW; //PW입력

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UButton* LoginMenu_btn_Register; //계정생성버튼

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UButton* LoginMenu_btn_Login; //로그인 버튼

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UButton* LoginMenu_btn_ForgetPW; //비번찾기 버튼

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UButton* LoginMenu_btn_Quit; //Quit버튼

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UButton* LoginMenu_btn_GuestLogin; //Guest로그인

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UTextBlock* LoginMenu_txt_FailedMSG; //Login실패 Msg

	UPROPERTY(EditDefaultsOnly, meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* ShowLoginMenuAnim; //UI Anim(Show)

	UPROPERTY(EditDefaultsOnly, meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* HideLoginMenuAnim; //UI ANnim(HIde)

	UPROPERTY(EditDefaultsOnly)
	class USoundWave* ButtonClickSound; //버튼클릭SFX

	UFUNCTION(BlueprintCallable)
	void OpenRegisterMenu(); //계정생성 메뉴 전환

	UFUNCTION(BlueprintCallable)
	void OnMyLogin(); //로그인 요청함수

	void SendLoginRequest(const FString& URL , const FString& JsonPayload , const FString& RequestType); //  로그인 HTTP 요청을 보내는 함수

	void OnLoginResponse(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful); // 로그인 HTTP 요청 결과 처리 함수

	UFUNCTION()
	void ShowLoginFailure(); //로그인 실패시 나타나는 Txt Msg 처리 함수
	UFUNCTION(BlueprintCallable)
	void OnClickedGuestLogin(); //게스트 로그인 (바로 로비로 입장)
	UFUNCTION(BlueprintCallable)
	void OnClickedQuit(); //Quit버튼 클릭(게임종료)

	//======================================================
	// 회원가입 Functions, Variables
	//======================================================

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UWidget* RegisterMenu; //Register 위젯

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UButton* RegisterMenu_btn_CreateID; //계정생성버튼

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UButton* RegisterMenu_btn_Back; //뒤로가기(로그인메뉴로)

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UEditableTextBox* RegisterMenu_txt_UserID; //가입ID입력

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UEditableTextBox* RegisterMenu_txt_UserPW; //가입PW입력
	
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UEditableTextBox* RegisterMenu_txt_UserPWCheck; //가입 PW Check입력

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UEditableTextBox* RegisterMenu_txt_NIckName; //가입 NickName 입력

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UImage* RegisterMenu_img_Success; //생성성공 이미지

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UImage* RegisterMenu_img_Failed; //생성실패 이미지
	
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UTextBlock* RegisterMenu_txt_Success; //생성성공 메시지
	
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UTextBlock* RegisterMenu_txt_Failed; //생성실패 메시지

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UButton* RegisterMenu_btn_Success; //생성성공 확인 버튼

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UButton* RegisterMenu_btn_Failed; //생성실패 확인 버튼

	UPROPERTY(EditDefaultsOnly, meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* ShowRegisterMenuAnim; //AI Anim(Show)
	
	UPROPERTY(EditDefaultsOnly, meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* HideRegisterMenuAnim; //AI Anim(HIde)

	UFUNCTION(BlueprintCallable)
	void OpenLoginMenu(); //로그인 화면 전환

	UFUNCTION(BlueprintCallable)
	void OnMyRegister(); //계정생성 요청함수

	void SendRegisterRequest(const FString& URL , const FString& JsonPayload , const FString& RequestType); // 계정생성 HTTP 요청을 보내는 함수

	void OnRegisterResponse(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful); //계정생성 HTTP요청결과 처리

	UFUNCTION()
	void ShowRegisterSuccess(); //계정생성 성공 UI 보여주기 함수
	UFUNCTION()
	void ShowRegisterFailed();  //계정생성 실패 UI 보여주기 함수
	UFUNCTION()
	void OnClickedRegisterSuccess(); //계정생성 성공UI 확인버튼바인딩
	UFUNCTION()
	void OnClickedRegisterFailed(); //계정생성 실패UI 확인버튼바인딩

};
