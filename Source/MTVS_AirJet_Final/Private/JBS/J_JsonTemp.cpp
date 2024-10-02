// Fill out your copyright notice in the Description page of Project Settings.


#include "JBS/J_JsonTemp.h"
#include "Engine/Engine.h"
#include "JBS/J_JsonUtility.h"

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

void AJ_JsonTemp::ReqPost(const FString &url)
{
	// json 만들기
	TMap<FString, FString> data;
	data.Add("name", "doremi");
	data.Add("age", "00");
	data.Add("height", "444");

	FString json = UJ_JsonUtility::MakeJson(data);
	FString resultURL = url;
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
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("요청 실패"));
	}
}