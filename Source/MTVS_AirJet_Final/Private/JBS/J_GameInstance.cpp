// Fill out your copyright notice in the Description page of Project Settings.


#include "JBS/J_GameInstance.h"
#include "Engine/Engine.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "JBS/J_JsonUtility.h"
#include "JBS/J_Utility.h"
#include "JsonObjectConverter.h"
#include "Math/UnrealMathUtility.h"
#include <Engine/World.h>

// 2. 요청 타입에 따라 다른 요청 실행
void UJ_GameInstance::RequestToServerByType(EJsonType type, const FString &sendJsonData)
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("%s 요청 시작"), *UEnum::GetValueAsString(type)));

	// 요청 타입 에 따라 다른 요청 실행
    switch(type)
    {
        // @@ 이 부분을 또 구조체로 만들어서 enum : struct 같은 느낌으로 가면 좋을지도?
		// 00 테스트용 콜백 요청 : 다른 것도 다 이런 형식임.
        case EJsonType::TEMP01_CALLBACK:
        {
			// 각 요청마다 정해진 딜리게이트에 Response 함수 바인드 | req 끝나면 이 함수가 호출되도록 함.
            tempDel.BindUObject(this, &UJ_GameInstance::ResTempCallback);
            // 3. 데이터 요청 : 반응 딜리게이트, 보낼 JsonData(없어도 그냥 TEXT("")으로 보냄), url, 기본 url 사용여부, 요청 타입(POST or GET)
            RequestData(tempDel, sendJsonData, TEXT("https://jsonplaceholder.typicode.com/posts"), false);
            break;
        }
        case EJsonType::SIGN_UP:
        {
            signupDel.BindUObject(this, &UJ_GameInstance::ResSignup);
            
            RequestData(signupDel, sendJsonData, TEXT("auth/signup"));
            break;
        }
        case EJsonType::LOGIN:
        {
            loginDel.BindUObject(this, &UJ_GameInstance::ResLogin);
            
            RequestData(loginDel, sendJsonData, TEXT("auth/login"));
            break;
        }
        case EJsonType::TEMP02_AUTH:
        {
            tempLoginAuthDel.BindUObject(this, &UJ_GameInstance::ResLoginAuth);
            RequestData(tempLoginAuthDel, sendJsonData, TEXT("test"));
            break;
        }
		case EJsonType::MISSION_DATA_RECEIVE:
        {
            missionDataGetDel.BindUObject(this, &UJ_GameInstance::ResMissionDataReceive);
            RequestData(missionDataGetDel, sendJsonData, TEXT("map/data"));
            break;
        }
    }
}

// 3. 서버에 요청
void UJ_GameInstance::RequestData(FResponseDelegate resDel, const FString& jsonData, const FString& url, bool useDefaultURL, ERequestType type)
{
	// 통신 모듈 생성
	FHttpModule* http = &FHttpModule::Get();

	// 요청 정보 설정
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> httpReq = http->CreateRequest();
	
	// 응답 함수 연결
	// 람다로 연결해서 정해놓은 반응 딜리게이트로 연결되도록함.
	httpReq->OnProcessRequestComplete().BindLambda([resDel, this](FHttpRequestPtr req, FHttpResponsePtr res, bool isSuccess)
	{
		// 성공 여부 확인
		if(isSuccess)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("요청 1단계 성공"));
			// 로그인 시에만 작동, 인증 값 저장해두기
			authorValue = res->GetHeader(TEXT("Authorization"));
			// GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("인증 값 : %s"), *authorValue));

			// json string 가져오기
			FString resStr = res->GetContentAsString();

			// 4. 딜리게이트에 바인드된 반응 함수 실행 
			resDel.ExecuteIfBound(resStr, isSuccess);
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("요청 실패"));
		}
	});
	// 기본 url 사용 여부에 따라 url 설정
	FString resultURL = useDefaultURL ? defaultURL + url : url;
	httpReq->SetURL(resultURL);

	// 전송 타입이 POST && 보낼 jsondata가 있다면 넣기
	if(type == ERequestType::POST && !jsonData.IsEmpty())
		httpReq->SetContentAsString(jsonData);

	// 요청 타입 설정 : 요청타입맵[enum] = TEXT("POST or GEt")
	httpReq->SetVerb(reqTypeMap[type]);
	// 헤더 설정
	httpReq->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	// 헤더에 인증값 설정 -> 로그인 이후 필요함.
	httpReq->SetHeader(TEXT("Authorization"), authorValue);

	// 서버 요청
	httpReq->ProcessRequest();
}

//XXX 웹 통신 요청 예시
void UJ_GameInstance::ReqTempCallback()
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

// 4. 반응 함수 예시
void UJ_GameInstance::ResTempCallback(const FString& jsonData, bool isSuccess)
{
	// json string 데이터 -> 구조체 데이터로 변환
	FResSimple jsonAry;
	FJsonObjectConverter::JsonObjectStringToUStruct<FResSimple>(jsonData, &jsonAry,0,0);

	// 변환된 구조체가지고 뭔가 하기
	auto rresult = jsonAry;
	// auto rresult = UJ_JsonUtility::JsonParseTempAry(resStr);

	// FString result;
	// for(auto r : rresult.tempJsons)
	// {
	// 	result.Append(r.ToString());
	// }
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString::Printf(TEXT("결과 : \n%s"), *rresult.ToString()));

	// GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString::Printf(TEXT("결과 : %s"), *result));
	// UE_LOG(LogTemp, Warning, TEXT("결과 : %s"), *result);
}

void UJ_GameInstance::ResSignup(const FString &jsonData, bool isSuccess)
{
	FResSimple resData;
	FJsonObjectConverter::JsonObjectStringToUStruct(jsonData, &resData,0,0);

	GEngine->AddOnScreenDebugMessage(-1, 31.f, FColor::Yellow, FString::Printf(TEXT("%s"), *resData.ToString()));

	signupResUseDelegate.ExecuteIfBound(resData);
}

// 로그인 반응 함수
void UJ_GameInstance::ResLogin(const FString &jsonData, bool isSuccess)
{
	// json 데이터 -> 구조체로 변경
	FLoginRes resData;
	FJsonObjectConverter::JsonObjectStringToUStruct(jsonData, &resData,0,0);

	GEngine->AddOnScreenDebugMessage(-1, 31.f, FColor::Yellow, FString::Printf(TEXT("%s"), *resData.ToString()));

	// 0.에서 바인드된 함수 실행
	loginResUseDelegate.ExecuteIfBound(resData);
}

void UJ_GameInstance::ResLoginAuth(const FString &jsonData, bool isSuccess)
{
	FResSimple resData;
	FJsonObjectConverter::JsonObjectStringToUStruct(jsonData, &resData,0,0);

	tempLoginAuthUseDel.ExecuteIfBound(resData);
}

void UJ_GameInstance::ResMissionDataReceive(const FString &jsonData, bool isSuccess)
{
	// GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("요청 2단계 성공"));
	FMissionDataRes resData;
	FJsonObjectConverter::JsonObjectStringToUStruct(jsonData, &resData,0,0);

	missionDataResDelegate.ExecuteIfBound(resData);
}



TSubclassOf<APawn> UJ_GameInstance::GetMissionPlayerPrefab()
{
	return playerPrefabMap[playerRole];
}