// Fill out your copyright notice in the Description page of Project Settings.


#include "JBS/J_JsonManager.h"
#include "Engine/Engine.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "JBS/J_JsonUtility.h"
#include "JsonObjectConverter.h"
#include "Math/UnrealMathUtility.h"
#include "Serialization/JsonTypes.h"
#include "Templates/SharedPointer.h"
#include "Templates/SharedPointerFwd.h"
#include <JBS/J_JsonUtility.h>

// Sets default values
AJ_JsonManager::AJ_JsonManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AJ_JsonManager::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AJ_JsonManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// template<typename InStructType>
// void AJ_JsonManager::ReqData(FResponseDelegate resDel, const InStructType& structData, const FString& url, bool useDefaultURL, ERequestType type)
// {
// 	// 구조체 -> json 화
// 	FString jsonData;
// 	FJsonObjectConverter::UStructToJsonObjectString(structData, jsonData, 0, 0);

// 	ReqData(resDel, url, useDefaultURL, type, jsonData);
// }

void AJ_JsonManager::ReqData(FResponseDelegate resDel, const FString& url, bool useDefaultURL, ERequestType type, const FString& jsonData)
{
	// 모듈 생성
	FHttpModule* http = &FHttpModule::Get();

	// 요청 정보 설정
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> httpReq = http->CreateRequest();
	
	// 응답 함수 연결
	httpReq->OnProcessRequestComplete().BindLambda([resDel](FHttpRequestPtr req, FHttpResponsePtr res, bool isSuccess)
	{
		if(isSuccess)
		{
			FString resStr = res->GetContentAsString();

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

	// 서버 요청
	httpReq->ProcessRequest();
}

//XXX 웹 통신 요청 예시
void AJ_JsonManager::ReqTempCallback()
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

void AJ_JsonManager::ResTempCallback(const FString& jsonData, bool isSuccess)
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

// template<typename InStructType>
// void AJ_JsonManager::RequestToServer(EJsonType type, const InStructType& structData)
// {
// 	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString::Printf(TEXT("%s 요청 시작"), *UEnum::GetValueAsString(type)));

// 	// type 에 따라 다른 req 실행
// 	switch(type)
// 	{
// 		// @@ 이 부분을 또 구조체로 만들어서 enum : struct 같은 느낌으로 가면 좋을지도?
// 		case EJsonType::TEMP01_CALLBACK:
// 		{
// 			tempDel.BindUObject(this, &AJ_JsonManager::ResTempCallback);
			
// 			ReqData<FTempJsonAry>(tempDel, structData, TEXT(""), false);
// 		}
// 			break;
// 	}
// }