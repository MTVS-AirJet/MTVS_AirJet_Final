// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <Http.h>
#include <JBS/J_Utility.h>
#include "JsonObjectConverter.h"
#include "J_JsonManager.generated.h"

DECLARE_DELEGATE_TwoParams(FResponseDelegate, const FString&, bool);

UCLASS()
class MTVS_AIRJET_FINAL_API AJ_JsonManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AJ_JsonManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


protected:
	// 기본 ip
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
	FString defaultURL = TEXT("");

	// 전송 타입 맵
	TMap<ERequestType, FString> reqTypeMap = {
		{ERequestType::GET, TEXT("GET")}
		,{ERequestType::POST, TEXT("POST")}
	};	


	// @@ 테스트용 딜리게이트
	FResponseDelegate tempDel;


public:

protected:
	// 서버에 요청
	// 구조체 데이터 있음 == 대부분 post 중 일부
	template<typename InStructType>
	void ReqData(FResponseDelegate resDel, const InStructType& structData, const FString& url = "", bool useDefaultURL = true, ERequestType type = ERequestType::POST)
	{
		// 구조체 -> json 화
		FString jsonData;
		FJsonObjectConverter::UStructToJsonObjectString(structData, jsonData, 0, 0);

		ReqData(resDel, url, useDefaultURL, type, jsonData);
	}

	// 구조체 데이터 없음 == get, post 일부
	void ReqData(FResponseDelegate resDel, const FString& url = "", bool useDefaultURL = true, ERequestType type = ERequestType::POST, const FString& jsonData = TEXT(""));

	// @@ 테스트용 데이터 주고 받기
	UFUNCTION(BlueprintCallable)
	void ReqTempCallback();

	UFUNCTION()
	void ResTempCallback(const FString& jsonData, bool isSuccess);

	

public:
	template<typename InStructType>
	void RequestToServer(EJsonType type, const InStructType& structData)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString::Printf(TEXT("%s 요청 시작"), *UEnum::GetValueAsString(type)));

		// type 에 따라 다른 req 실행
		switch(type)
		{
			// @@ 이 부분을 또 구조체로 만들어서 enum : struct 같은 느낌으로 가면 좋을지도?
			case EJsonType::TEMP01_CALLBACK:
			{
				tempDel.BindUObject(this, &AJ_JsonManager::ResTempCallback);
				
				ReqData<FTempJsonAry>(tempDel, structData, TEXT("https://jsonplaceholder.typicode.com/posts"), false);
			}
				break;
		}
	}
	
};
