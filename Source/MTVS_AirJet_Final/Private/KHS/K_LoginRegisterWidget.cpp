// Fill out your copyright notice in the Description page of Project Settings.


#include "KHS/K_LoginRegisterWidget.h"
#include "KHS/K_GameInstance.h"
#include "Json.h"
#include "JsonUtilities.h"
#include "Serialization/JsonSerializer.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Delegates/Delegate.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/Widget.h"
#include "Components/WidgetSwitcher.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Engine/TimerHandle.h"


//생성자&초기화
bool UK_LoginRegisterWidget::Initialize()
{
	Super::Initialize();

	// 초기화 =====================================================
	
	if ( RegisterMenu_img_Success )
		RegisterMenu_img_Success->SetVisibility(ESlateVisibility::Hidden); //Register 성공 이미지 숨김
	if ( RegisterMenu_img_Failed )
		RegisterMenu_img_Failed->SetVisibility(ESlateVisibility::Hidden); //Register 실패 이미지 숨김
	if ( RegisterMenu_btn_Success ) {
		RegisterMenu_btn_Success->SetVisibility(ESlateVisibility::Hidden); //Register 성공 버튼 숨김 및 바인딩
		RegisterMenu_btn_Success->OnClicked.AddDynamic(this , &UK_LoginRegisterWidget::OnClickedRegisterSuccess);
	}
	if ( RegisterMenu_btn_Failed )	{
		RegisterMenu_btn_Failed->SetVisibility(ESlateVisibility::Hidden);//Register 실패 버튼 숨김 및 바인딩
		RegisterMenu_btn_Failed->OnClicked.AddDynamic(this , &UK_LoginRegisterWidget::OnClickedRegisterFailed);
	}
	if ( RegisterMenu_txt_Failed ) {
		RegisterMenu_txt_Failed->SetVisibility(ESlateVisibility::Hidden); //Register 실패 Txtbox 숨김
	}
	if ( LoginMenu_txt_FailedMSG ) {
		LoginMenu_txt_FailedMSG->SetVisibility(ESlateVisibility::Hidden); //Login 실패 Txtbox 숨김
	}
	
	// 버튼클릭 이벤트 바인딩 ======================================
	// Login 메뉴 버튼 델리게이트 바인딩
	if ( LoginMenu_btn_Register ) //계정생성버튼
		LoginMenu_btn_Register->OnClicked.AddDynamic(this , &UK_LoginRegisterWidget::OpenRegisterMenu);
	if ( LoginMenu_btn_Login ) //로그인 버튼
		LoginMenu_btn_Login->OnClicked.AddDynamic(this , &UK_LoginRegisterWidget::OnMyLogin);
	if ( LoginMenu_btn_GuestLogin ) //Guest로그인
		LoginMenu_btn_GuestLogin->OnClicked.AddDynamic(this , &UK_LoginRegisterWidget::OnClickedGuestLogin);
	if (LoginMenu_btn_Quit) // Quit버튼
		LoginMenu_btn_Quit->OnClicked.AddDynamic(this, &UK_LoginRegisterWidget::OnClickedQuit);

	// Register 메뉴 버튼 델리게이트 바인딩
	if ( RegisterMenu_btn_CreateID ) //계정 생성 버튼
		RegisterMenu_btn_CreateID->OnClicked.AddDynamic(this , &UK_LoginRegisterWidget::OnMyRegister);
	if ( RegisterMenu_btn_Back ) //계정 생성 버튼
		RegisterMenu_btn_Back->OnClicked.AddDynamic(this , &UK_LoginRegisterWidget::OpenLoginMenu);
	if ( RegisterMenu_btn_Success ) //계정 생성 성공 확인 버튼
		RegisterMenu_btn_Success->OnClicked.AddDynamic(this , &UK_LoginRegisterWidget::OnClickedRegisterSuccess);
	if ( RegisterMenu_btn_Failed ) //계정 생성 실패 확인 버튼
		RegisterMenu_btn_Failed->OnClicked.AddDynamic(this , &UK_LoginRegisterWidget::OnClickedRegisterFailed);
			
	return true;
}

#pragma region Login Menu

//계정생성 메뉴 전환
void UK_LoginRegisterWidget::OpenRegisterMenu()
{
	if ( HideLoginMenuAnim )
		PlayAnimation(HideLoginMenuAnim);

	FTimerHandle TimerHandle_MenuSwitch;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_MenuSwitch , [this]()
	{
		if ( MenuSwitcher )
			MenuSwitcher->SetActiveWidget(RegisterMenu);
		PlayAnimation(ShowRegisterMenuAnim);
	} , 1.0f , false);

	UE_LOG(LogTemp , Log , TEXT("OpenRegisterMenu called"));
}

//로그인 요청함수
void UK_LoginRegisterWidget::OnMyLogin()
{
	// 1. 입력된 정보를 가져옴
	FString LoginUserID = LoginMenu_txt_UserID->GetText().ToString();
	FString LoginPassword = LoginMenu_txt_UserPW->GetText().ToString();

	// 2. 로그인 정보를 서버로 전송할 URL 설정
	FString URL = LoginURL; // 백엔드 서버 URL

	//// 3. JSON 객체 생성 후 입력된 정보 추가
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
	if ( false == JsonObject.IsValid() )
	{
		UE_LOG(LogTemp , Warning , TEXT("Failed to Create JsonObject"));
	}

	JsonObject->SetStringField("loginId" , LoginUserID);
	JsonObject->SetStringField("password" , LoginPassword);

	// 4. JSON 데이터를 서버가 이해할 수 있는 문자열로 변환
	FString JsonPayload; // 데이터를 담을 JsonPayload 라는 빈 문자열 생성

	// 5. JSON 데이터를 문자열로 바꾸는 도구(Writer)를 생성
	TSharedRef<TJsonWriter<TCHAR , TCondensedJsonPrintPolicy<TCHAR>>> Writer = TJsonWriterFactory<TCHAR , TCondensedJsonPrintPolicy<TCHAR>>::Create(&JsonPayload);

	// 6. JSON 객체를 문자열로 변환하여 JsonPayload에 저장
	FJsonSerializer::Serialize(JsonObject.ToSharedRef() , Writer);
	if ( false == FJsonSerializer::Serialize(JsonObject.ToSharedRef() , Writer) )
	{
		UE_LOG(LogTemp , Warning , TEXT("Failed to serialize JSON object"));
		return;
	}

	// 7. HTTP 요청을 POST 방식으로 서버로 전송
	SendLoginRequest(URL , JsonPayload , "POST");
}

// 로그인 HTTP 요청을 보내는 함수
void UK_LoginRegisterWidget::SendLoginRequest(const FString& URL , const FString& JsonPayload , const FString& RequestType)
{
	// 1. HTTP 요청 객체를 생성
	TSharedRef<IHttpRequest , ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();

	// 2. 요청이 완료되면 호출될 함수 지정 (응답을 처리하는 함수 바인딩)
	Request->OnProcessRequestComplete().BindUObject(this , &UK_LoginRegisterWidget::OnLoginResponse);

	// 3. 요청할 URL 설정
	FString RequestURL = LoginURL; // 요청을 처리할 URL 설정
	Request->SetURL(RequestURL);

	// 4. 요청 타입 설정 (Post, Get 등)
	Request->SetVerb("POST");

	// 5. 요청 헤더에 데이터를 어떤 형식으로 전송할지 설정 (JSON 등)
	Request->SetHeader("Content-Type" , "application/json");

	// 6. 서버에 보낼 데이터를 담아 요청에 설정
	Request->SetContentAsString(JsonPayload);

	// 7. 설정한 요청을 서버로 전송
	Request->ProcessRequest();
}

// 로그인 HTTP 요청 결과 처리 함수
void UK_LoginRegisterWidget::OnLoginResponse(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful)
{
	// 1. 요청 성공과 응답성 유효에 대해 확인
	if ( false == bWasSuccessful || false == Response.IsValid() )
	{
		UE_LOG(LogTemp , Warning , TEXT("Login request failed")); // 요청 실패 로그
		ShowLoginFailure();
		return;
	}
	// 2. 서버로부터 받은 응답을 문자열로 가져옴
	FString ResponseContent = Response->GetContentAsString();
	UE_LOG(LogTemp , Log , TEXT("Login Response: %s") , *ResponseContent);  // 받은 응답을 로그로 출력

	// 응답받은 헤더 로그출력(토큰 확인용)
	TArray<FString> Headers = Response->GetAllHeaders();
	UE_LOG(LogTemp , Log , TEXT("Response Headers:"));
	for ( const FString& Header : Headers )
	{
		UE_LOG(LogTemp , Log , TEXT("%s") , *Header);
	}

	// 유저 토큰 헤더에서 값 따로 저장
	FString AuthToken;
	if ( Response->GetHeader(TEXT("Authorization")).IsEmpty() == false )
	{
		AuthToken = Response->GetHeader(TEXT("Authorization"));
		UE_LOG(LogTemp , Log , TEXT("Authorization Token: %s") , *AuthToken);

		// GameInstance에 토큰 저장
		if ( UK_GameInstance* GameInstance = Cast<UK_GameInstance>(GetWorld()->GetGameInstance()) )
		{
			GameInstance->SetAuthToken(AuthToken);
			UE_LOG(LogTemp , Log , TEXT("Auth Token saved to GameInstance"));
		}
	}
	else {
		UE_LOG(LogTemp , Warning , TEXT("Authorization header not found"));
	}

	// 3. JSON 응답을 처리하기 위해 JSON 객체 생성
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseContent);

	// 4. JSON 응답을 파싱(구문 해석), 성공적으로 파싱 시, JsonObject에 데이터 삽입
	if ( FJsonSerializer::Deserialize(Reader , JsonObject) )
	{
		// 5. 서버 응답에서 "success" 라는 필드를 찾아, 성공 여부를 확인
		bool bSuccess = JsonObject->GetBoolField(TEXT("success"));

		if ( bSuccess ) // 로그인에 성공 시, 게임 인스턴스를 가져와 ServerWidgetMap으로 이동
		{
			// 로그인 성공 처리
			UK_GameInstance* GameInstance = Cast<UK_GameInstance>(GetWorld()->GetGameInstance());
			if ( GameInstance )
			{
				// UserId 임시 저장
				FString UserId;
				if ( JsonObject->TryGetStringField(TEXT("loginId") , UserId) )
				{
					GameInstance->SetUserId(UserId);
				}

				GameInstance->ContinueCurrentSound(); // 로그인 성공 시에도 현재 사운드 계속 재생
				GameInstance->TravelMainLobbyMap(true); // true 인자를 통해 현재 사운드를 유지하며 이동
			}
			else {
				UE_LOG(LogTemp , Error , TEXT("Failed to get GameInstance"));
				ShowLoginFailure();
			}
		}
		else {
			// 로그인 실패 처리
			FString errorMessage;
			if ( JsonObject->TryGetStringField(TEXT("response") , errorMessage) )
			{
				UE_LOG(LogTemp , Warning , TEXT("Login Failed: %s") , *errorMessage);
			}
			ShowLoginFailure();
		}
	}
	else {
		UE_LOG(LogTemp , Warning , TEXT("Unexpected response format"));
		ShowLoginFailure();
	}
}

//로그인 실패시 나타나는 Txt Msg 처리함수
void UK_LoginRegisterWidget::ShowLoginFailure()
{
	if ( LoginMenu_txt_FailedMSG )
		LoginMenu_txt_FailedMSG->SetVisibility(ESlateVisibility::Visible);
}

//게스트 로그인 (바로 로비로 입장)
void UK_LoginRegisterWidget::OnClickedGuestLogin()
{
	UK_GameInstance* GameInstance = Cast<UK_GameInstance>(GetWorld()->GetGameInstance());
	if ( GameInstance )
	{
		GameInstance->TravelMainLobbyMap(true); // true 인자를 통해 현재 사운드를 유지하며 레벨로 트래블
	}
}

//Quit버튼 클릭(게임종료)
void UK_LoginRegisterWidget::OnClickedQuit()
{
	UWorld* World = GetWorld();
	APlayerController* pc = World->GetFirstPlayerController();

	pc->ConsoleCommand("quit"); // 게임종료명령
}

#pragma endregion

#pragma region Register Menu

//로그인 화면 전환
void UK_LoginRegisterWidget::OpenLoginMenu()
{
	if ( HideRegisterMenuAnim )
	{
		PlayAnimation(HideRegisterMenuAnim);
	}

	FTimerHandle TimerHandle_MenuSwitch;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_MenuSwitch , [this]()
	{
		if ( MenuSwitcher )
			MenuSwitcher->SetActiveWidget(LoginMenu);
		PlayAnimation(ShowLoginMenuAnim);

	} , 1.0f , false);

	UE_LOG(LogTemp , Log , TEXT("OpenLoginMenu called"));
}

//계정생성 요청함수
void UK_LoginRegisterWidget::OnMyRegister()
{
	// 입력된 회원가입 정보를 가져옴.
	FString RegisterUserID = RegisterMenu_txt_UserID->GetText().ToString();
	FString RegisterNickname = RegisterMenu_txt_NIckName->GetText().ToString();
	FString RegisterPassword = RegisterMenu_txt_UserPW->GetText().ToString();
	FString RegisterPasswordCheck = RegisterMenu_txt_UserPWCheck->GetText().ToString();

	// 계정생성 정보를 서버로 전송할 URL 설정
	FString URL = SignupURL; // 백엔드 서버 Signup URL

	// JSON 객체(Object) 생성 후 입력된 정보 추가
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject()); // 새로운 Json 객체 생성

	if ( false == JsonObject.IsValid() )
	{
		UE_LOG(LogTemp , Warning , TEXT("Failed to Create JsonObject"))
	}

	JsonObject->SetStringField("loginId" , RegisterUserID); // UserID 텍스트를 받아서 "user_id" 키에 저장
	JsonObject->SetStringField("nickname" , RegisterNickname); // Nickname 텍스트를 받아서 "nickname" 키에 저장
	JsonObject->SetStringField("password" , RegisterPassword); // Password 텍스트를 받아서 "password" 키에 저장
	JsonObject->SetStringField("confirmPassword" , RegisterPasswordCheck); // PasswordCheck 텍스트를 받아서 "confirmPassword" 키에 저장

	// JSON 데이터를 서버가 이해할 수 있는 문자열로 변환
	FString JsonPayload; // 데이터를 담을 JsonPayload 라는 빈 문자열 생성

	// JSON 데이터를 문자열로 바꾸는 도구(Writer)를 생성
	TSharedRef<TJsonWriter<TCHAR , TCondensedJsonPrintPolicy<TCHAR>>> Writer = TJsonWriterFactory<TCHAR , TCondensedJsonPrintPolicy<TCHAR>>::Create(&JsonPayload);

	FJsonSerializer::Serialize(JsonObject.ToSharedRef() , Writer); // JSON 객체를 문자열로 변환하여 JsonPayload에 저장
	if ( false == FJsonSerializer::Serialize(JsonObject.ToSharedRef() , Writer) )
	{
		UE_LOG(LogTemp , Warning , TEXT("Failed to serialize JSON object"));
		return;
	}

	// HTTP 요청을 POST 방식으로 서버로 전송
	SendRegisterRequest(URL , JsonPayload , "POST");
}

// 계정생성 HTTP 요청을 보내는 함수
void UK_LoginRegisterWidget::SendRegisterRequest(const FString& URL , const FString& JsonPayload , const FString& RequestType)
{
	// 1. HTTP 요청 객체를 생성
	TSharedRef<IHttpRequest , ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	// 2. 요청이 완료되면 호출될 함수 지정 (응답을 처리하는 함수 바인딩)
	Request->OnProcessRequestComplete().BindUObject(this , &UK_LoginRegisterWidget::OnRegisterResponse);
	// 3. 요청할 URL 설정
	FString RequestURL = SignupURL; // 백엔드 서버 Signup URL
	Request->SetURL(RequestURL);
	// 4. 요청 타입 설정 (Post, Get 등)
	Request->SetVerb("POST");
	// 5. 요청 헤더에 데이터를 어떤 형식으로 전송할지 설정 (JSON 등)
	Request->SetHeader("Content-Type" , "application/json");
	// 6. 서버에 보낼 데이터를 담아 요청에 설정
	Request->SetContentAsString(JsonPayload);
	// 7. 설정한 요청을 서버로 전송
	Request->ProcessRequest();
}

//계정생성 HTTP요청결과 처리
void UK_LoginRegisterWidget::OnRegisterResponse(FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful)
{
	// 1. 요청 성공과 응답성 유효에 대해 확인
	if ( false == bWasSuccessful || false == Response.IsValid() )
	{
		UE_LOG(LogTemp , Warning , TEXT("Register request failed")); // 요청 실패 로그
		ShowRegisterFailed();
		return;
	}

	// 2. 서버로부터 받은 응답을 문자열로 가져옴
	FString ResponseContent = Response->GetContentAsString();
	UE_LOG(LogTemp , Log , TEXT("HTTP Response: %s") , *ResponseContent);  // 받은 응답을 로그로 출력

	// 3. JSON 응답을 처리하기 위해 JSON 객체 생성
	TSharedPtr<FJsonObject> JsonObject;

	// 4. 응답 내용을 Json 형식으로 읽기 위한 리더 객체 생성
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseContent);

	// 5. JSON 응답을 파싱(구문 해석), 성공적으로 파싱 시, JsonObject에 데이터 삽입

	if ( FJsonSerializer::Deserialize(Reader , JsonObject) )
	{
		// 6. 서버 응답에서 "success" 라는 필드를 찾아, 성공 여부를 확인
		if ( JsonObject->HasField(TEXT("success")) )
		{
			bool bSuccess = JsonObject->GetBoolField(TEXT("success"));
			/*FString RegisterResponse;
			JsonObject->TryGetStringField(TEXT("success"), RegisterResponse);*/
			if ( bSuccess )
			{
				ShowRegisterSuccess(); // 계정생성에 성공
			}
			else {
				FString errorMessage;
				if ( JsonObject->TryGetStringField(TEXT("response") , errorMessage) )
				{
					RegisterMenu_txt_Failed->SetText(FText::FromString(errorMessage)); //계정생성 실패시, 에러메시지 출력
					UE_LOG(LogTemp , Warning , TEXT("Register Failed: %s") , *errorMessage);
				}
				ShowRegisterFailed();
			}
		}
		else {
			UE_LOG(LogTemp , Warning , TEXT("Unexpected response format"));
			ShowRegisterFailed();
		}
	}
	else {
		UE_LOG(LogTemp , Warning , TEXT("Failed to parse JSON response"));
		ShowRegisterFailed();
	}
}

//계정생성 성공 UI보여주기 함수
void UK_LoginRegisterWidget::ShowRegisterSuccess()
{
	if ( RegisterMenu_img_Success ) // "계정이 생성되었습니다." 이미지 송출
		RegisterMenu_img_Success->SetVisibility(ESlateVisibility::Visible);
	if ( RegisterMenu_btn_Success ) // 확인 버튼 보이게
		RegisterMenu_btn_Success->SetVisibility(ESlateVisibility::Visible);
}

//계정생성 실패 UI보여주기 함수
void UK_LoginRegisterWidget::ShowRegisterFailed()
{
	// 0.1초 후에 UI를 표시
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle , [this]()
	{
		if ( RegisterMenu_img_Failed ) 	{
			RegisterMenu_img_Failed->SetVisibility(ESlateVisibility::Visible);
		}
		if ( RegisterMenu_btn_Failed )	{
			RegisterMenu_btn_Failed->SetVisibility(ESlateVisibility::Visible);
		}
		if ( RegisterMenu_txt_Failed ) {
			RegisterMenu_txt_Failed->SetVisibility(ESlateVisibility::Visible);
		}
		UE_LOG(LogTemp , Warning , TEXT("ShowRegisterFailureUI executed after delay"));
	} , 0.1f , false);
}

//계정생성 성공UI 확인버튼바인딩
void UK_LoginRegisterWidget::OnClickedRegisterSuccess()
{
	// 계정생성 성공 시 나타나는 확인 버튼을 누를경우,
	OpenLoginMenu();
	// 성공 안내 관련 위젯 모두 숨김
	if ( RegisterMenu_img_Success ) 
		RegisterMenu_img_Success->SetVisibility(ESlateVisibility::Hidden);
	if ( RegisterMenu_btn_Success )
		RegisterMenu_btn_Success->SetVisibility(ESlateVisibility::Hidden);
}

//계정생성 실패UI 확인버튼바인딩
void UK_LoginRegisterWidget::OnClickedRegisterFailed()
{
	// 실패 안내 관련 위젯 모두 숨김
	if ( RegisterMenu_img_Failed ) 
		RegisterMenu_img_Failed->SetVisibility(ESlateVisibility::Hidden);
	if ( RegisterMenu_btn_Failed )
		RegisterMenu_btn_Failed->SetVisibility(ESlateVisibility::Hidden);
	if ( RegisterMenu_txt_Failed )
		RegisterMenu_txt_Failed->SetVisibility(ESlateVisibility::Hidden);
}

#pragma endregion
