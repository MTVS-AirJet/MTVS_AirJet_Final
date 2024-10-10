// Fill out your copyright notice in the Description page of Project Settings.


#include "KHS/K_GameInstance.h"
#include "KHS/K_LoginRegisterWidget.h"

#include "JBS/J_JsonUtility.h"
#include "JBS/J_Utility.h"

#include "Components/AudioComponent.h"
#include <Engine/World.h>
#include "Engine/Engine.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "JsonObjectConverter.h"
#include "Math/UnrealMathUtility.h"
#include "Templates/SubclassOf.h"
#include "Kismet/GameplayStatics.h"

void UK_GameInstance::Host(FString ServerName)
{
}

void UK_GameInstance::Join(uint32 Index)
{
}

void UK_GameInstance::CreateSession()
{
}

void UK_GameInstance::RefreshServerList()
{
}

// 로그인 UI를 생성하는 함수
void UK_GameInstance::CreateLoginWidget()
{
	// LoginWidgetFactory를 통해 LogInUI 위젯 생성
	LoginWidget = CreateWidget<UK_LoginRegisterWidget>(this , LoginWidgetFactory);
	LoginWidget->SetInterface(this); //부모함수 호출
	LoginWidget->SetUI(); //부모함수 호출
}

void UK_GameInstance::LoadServerWidgetMap(bool bKeepCurrentSound)
{
}

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

#pragma region BackEnd Coordination
void UK_GameInstance::SetUserId(const FString& NewUserId)
{	
	UserId = NewUserId;
}

FString UK_GameInstance::GetUserId() const
{
	return UserId;
}
#pragma endregion




#pragma region 여기부터 JBS 작성 단
void UK_GameInstance::ReqData(FResponseDelegate resDel, const FString& jsonData, const FString& url, bool useDefaultURL, ERequestType type)
{
	// 모듈 생성
	FHttpModule* http = &FHttpModule::Get();

	// 요청 정보 설정
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> httpReq = http->CreateRequest();
	
	// 응답 함수 연결
	httpReq->OnProcessRequestComplete().BindLambda([resDel, this](FHttpRequestPtr req, FHttpResponsePtr res, bool isSuccess)
	{
		if(isSuccess)
		{
			FString resStr = res->GetContentAsString();
			// @@ 로그인 시에 인증 값 저장해두기 인스턴스
			authorValue = res->GetHeader(TEXT("Authorization"));

			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("인증 값 : %s"), *authorValue));

			resDel.ExecuteIfBound(resStr, isSuccess)	;
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("요청 실패"));
		}
	});
	// url 설정
	FString resultURL = useDefaultURL ? defaultURL + url : url;
	httpReq->SetURL(resultURL);
	// 보낼 jsondata가 있다면 넣기
	if(type == ERequestType::POST && !jsonData.IsEmpty())
	{
		httpReq->SetContentAsString(jsonData);
	}
	// 요청 타입 설정
	httpReq->SetVerb(reqTypeMap[type]);
	// 헤더 설정
	httpReq->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	httpReq->SetHeader(TEXT("Authorization"), authorValue);

	// 서버 요청
	httpReq->ProcessRequest();
}

//XXX 웹 통신 요청 예시
void UK_GameInstance::ReqTempCallback()
{
	// json으로 보낼 구조체 준비
	FTempJsonAry jsonAry;
	for(int i = 0; i < 3; i++)
	{
		FTempJson temp;
		temp.id = FMath::RandRange(0, 999);
		temp.userId = FMath::RandRange(1000, 9999);
		temp.title = TEXT("doremi");
		temp.body = TEXT("farondo");

		jsonAry.tempJsons.Add(temp);
	}

	// 내부에서 jsonManager 찾은 후 EJsonType에 해당하는 Req 함수 실행
	UJ_JsonUtility::RequestExecute<FTempJsonAry>(GetWorld(), EJsonType::TEMP01_CALLBACK, jsonAry);
}

void UK_GameInstance::ResTempCallback(const FString& jsonData, bool isSuccess)
{
	FTempJsonAry jsonAry;
	FJsonObjectConverter::JsonObjectStringToUStruct<FTempJsonAry>(jsonData, &jsonAry,0,0);

	// 변환된 구조체가지고 뭔가 하기
	auto rresult = jsonAry;
	// auto rresult = UJ_JsonUtility::JsonParseTempAry(resStr);

	FString result;
	for(auto r : rresult.tempJsons)
	{
		result.Append(r.ToString());
	}
	

	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString::Printf(TEXT("결과 : %s"), *result));
	UE_LOG(LogTemp, Warning, TEXT("결과 : %s"), *result);
}

void UK_GameInstance::ResSignup(const FString &jsonData, bool isSuccess)
{
	FResSimple resData;
	FJsonObjectConverter::JsonObjectStringToUStruct(jsonData, &resData,0,0);

	GEngine->AddOnScreenDebugMessage(-1, 31.f, FColor::Yellow, FString::Printf(TEXT("%s"), *resData.ToString()));
}

void UK_GameInstance::ResLogin(const FString &jsonData, bool isSuccess)
{
	FResSimple resData;
	FJsonObjectConverter::JsonObjectStringToUStruct(jsonData, &resData,0,0);

	GEngine->AddOnScreenDebugMessage(-1, 31.f, FColor::Yellow, FString::Printf(TEXT("%s"), *resData.ToString()));
}

void UK_GameInstance::ResLoginAuth(const FString &jsonData, bool isSuccess)
{
	FResSimple resData;
	FJsonObjectConverter::JsonObjectStringToUStruct(jsonData, &resData,0,0);

	tempLoginAuthUseDel.ExecuteIfBound(resData);
}

void UK_GameInstance::RequestToServer(EJsonType type, const FString &sendJsonData)
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("%s 요청 시작"), *UEnum::GetValueAsString(type)));

	// type 에 따라 다른 req 실행
    switch(type)
    {
        // @@ 이 부분을 또 구조체로 만들어서 enum : struct 같은 느낌으로 가면 좋을지도?
        case EJsonType::TEMP01_CALLBACK:
        {
            tempDel.BindUObject(this, &UK_GameInstance::ResTempCallback);
            
            ReqData(tempDel, sendJsonData, TEXT("https://jsonplaceholder.typicode.com/posts"), false);
            break;
        }
        case EJsonType::SIGN_UP:
        {
            signupDel.BindUObject(this, &UK_GameInstance::ResSignup);
            
            ReqData(signupDel, sendJsonData, TEXT("auth/signup"));
            break;
        }
        case EJsonType::LOGIN:
        {
            loginDel.BindUObject(this, &UK_GameInstance::ResLogin);
            
            ReqData(loginDel, sendJsonData, TEXT("auth/login"));
            break;
        }
        case EJsonType::TEMP02_AUTH:
        {
            tempLoginAuthDel.BindUObject(this, &UK_GameInstance::ResLoginAuth);
            ReqData(tempLoginAuthDel, sendJsonData, TEXT("test"));
            break;
        }
    }
}

#pragma endregion

TSubclassOf<APawn> UK_GameInstance::GetMissionPlayerPrefab()
{
	return playerPrefabMap[playerRole];
}
