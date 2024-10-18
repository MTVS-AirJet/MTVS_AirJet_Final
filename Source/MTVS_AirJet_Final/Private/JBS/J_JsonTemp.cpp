// Fill out your copyright notice in the Description page of Project Settings.


#include "JBS/J_JsonTemp.h"
#include "Engine/Engine.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "JBS/J_JsonUtility.h"
#include "JsonObjectConverter.h"
#include "Logging/LogMacros.h"
#include "Logging/LogVerbosity.h"
#include "Templates/SharedPointer.h"
#include "Templates/SharedPointerFwd.h"

// Sets default values
AJ_JsonTemp::AJ_JsonTemp()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AJ_JsonTemp::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AJ_JsonTemp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AJ_JsonTemp::ReqGet(const FString& url, bool useDefaultUrl)
{
	// 모듈 생성
	FHttpModule* http = &FHttpModule::Get();

	// 요청 정보 설정
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> httpReq = http->CreateRequest();

	// 응답 함수 연결
	httpReq->OnProcessRequestComplete().BindUObject(this, &AJ_JsonTemp::ResGet);
	FString resultURL = useDefaultUrl ? defaultUrl + url : url;
	httpReq->SetURL(resultURL);
	httpReq->SetVerb(TEXT("GET"));
	httpReq->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	// 서버 요청
	httpReq->ProcessRequest();

}

void AJ_JsonTemp::ResGet(FHttpRequestPtr req, FHttpResponsePtr res, bool isSuccess)
{
	// if(isSuccess && res.IsValid())
	// {
	// 	FString resStr = res->GetContentAsString();

	// 	FTempJsonAry result = UJ_JsonUtility::JsonParseTemp(resStr);
	// 	// FString result = resStr;

	// 	FString rresult = TEXT("");
	// 	for(FTempJson tempJson : result.tempJsons)
	// 	{
	// 		rresult.Append(tempJson.ToString());
	// 	}

	// 	// GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString::Printf(TEXT("%s"), *result));
	// 	UE_LOG(LogTemp, Warning, TEXT("결과 : %s"), *rresult);
	// }
	// else {
	// 	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("요청 실패"));
	// }
}

void AJ_JsonTemp::ReqPost(const FString &url, bool useDefaultUrl)
{
	// json 만들기
	TMap<FString, FString> data;
	data.Add("name", "doremi");
	data.Add("age", "00");
	data.Add("height", "444");

	FString json = UJ_JsonUtility::MakeJson(data);
	FString resultURL = useDefaultUrl ? defaultUrl + url : url;
	FHttpModule* http = &FHttpModule::Get();

	// 요청 정보 설정
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> httpReq = http->CreateRequest();

	// 응답 함수 연결
	httpReq->OnProcessRequestComplete().BindUObject(this, &AJ_JsonTemp::ResPost);
	httpReq->SetContentAsString(json);
	httpReq->SetURL(resultURL);
	httpReq->SetVerb(TEXT("POST"));
	httpReq->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	// 서버 요청
	httpReq->ProcessRequest();
}

void AJ_JsonTemp::ResPost(FHttpRequestPtr req, FHttpResponsePtr res, bool isSuccess)
{
	if(isSuccess && res.IsValid())
	{
		FString resStr = res->GetContentAsString();

		FString result = resStr;

		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString::Printf(TEXT("%s"), *result));
		UE_LOG(LogTemp, Warning, TEXT("결과 : %s"), *result);
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("요청 실패"));
	}
}

// solved
void AJ_JsonTemp::ReqPostTemp(const FString &url, bool useDefaultUrl)
{
	// json 만들기
		FTempJson tempJson;
		tempJson.userId = FMath::RandRange(123, 999);
		tempJson.id = 12;
		tempJson.title = TEXT("doremi");
		tempJson.body = TEXT("farondo");


	FString json;
	FJsonObjectConverter::UStructToJsonObjectString(tempJson, json,0,0);


	FString resultURL = useDefaultUrl ? defaultUrl + url : url;
	FHttpModule* http = &FHttpModule::Get();

	// 요청 정보 설정
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> httpReq = http->CreateRequest();

	// 응답 함수 연결
	httpReq->OnProcessRequestComplete().BindUObject(this, &AJ_JsonTemp::ResPostTemp);
	httpReq->SetContentAsString(json);
	httpReq->SetURL(resultURL);
	httpReq->SetVerb(TEXT("POST"));
	httpReq->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	// 서버 요청
	httpReq->ProcessRequest();
}

void AJ_JsonTemp::ResPostTemp(FHttpRequestPtr req, FHttpResponsePtr res, bool isSuccess)
{
	if(isSuccess && res.IsValid())
	{
		FString resStr = res->GetContentAsString();

		auto rresult = UJ_JsonUtility::JsonParseTemp(resStr);

		FString result = rresult.ToString();
		

		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString::Printf(TEXT("%s"), *result));
		UE_LOG(LogTemp, Warning, TEXT("결과 : %s"), *result);
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("요청 실패"));
	}
}

// solved
void AJ_JsonTemp::ReqPostTempAry(const FString &url, bool useDefaultUrl)
{
	// json 만들기
	FTempJsonAry temps;
	for(int i = 0; i < 3; i++)
	{
		FTempJson tempJson;
		tempJson.userId = FMath::RandRange(123, 999);
		tempJson.id = 12;
		tempJson.title = TEXT("doremi");
		tempJson.body = TEXT("farondo");
		
		temps.tempJsons.Add(tempJson);
	}


	FString json;
	FJsonObjectConverter::UStructToJsonObjectString(temps, json,0,0);


	FString resultURL = useDefaultUrl ? defaultUrl + url : url;
	FHttpModule* http = &FHttpModule::Get();

	// 요청 정보 설정
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> httpReq = http->CreateRequest();

	// 응답 함수 연결
	httpReq->OnProcessRequestComplete().BindUObject(this, &AJ_JsonTemp::ResPostTempAry);
	httpReq->SetContentAsString(json);
	httpReq->SetURL(resultURL);
	httpReq->SetVerb(TEXT("POST"));
	httpReq->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	// 서버 요청
	httpReq->ProcessRequest();
}

void AJ_JsonTemp::ResPostTempAry(FHttpRequestPtr req, FHttpResponsePtr res, bool isSuccess)
{
	if(isSuccess && res.IsValid())
	{
		// json -> 정해진 구조체
		FString resStr = res->GetContentAsString();

		FTempJsonAry jsonAry;
		FJsonObjectConverter::JsonObjectStringToUStruct<FTempJsonAry>(resStr, &jsonAry,0,0);

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
	else {
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("요청 실패"));
	}
}