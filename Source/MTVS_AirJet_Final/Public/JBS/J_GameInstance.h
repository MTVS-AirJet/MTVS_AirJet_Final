// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include <JBS/J_Utility.h>
#include <JsonObjectConverter.h>
#include <JBS/J_JsonManager.h>
#include "J_GameInstance.generated.h"

/**
 * 
 */
DECLARE_DELEGATE_TwoParams(FResponseDelegate, const FString&, bool);

DECLARE_DELEGATE_OneParam(FResSimpleDelegate, const FResSimple&);

// _One~ _NineParams 까지 가능

UCLASS()
class MTVS_AIRJET_FINAL_API UJ_GameInstance : public UGameInstance
{
	GENERATED_BODY()
	
protected:
	// 기본 ip
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
	FString defaultURL = TEXT("http://125.132.216.190:7757/api/");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
	FString authorValue = TEXT("");

	// 전송 타입 맵
	TMap<ERequestType, FString> reqTypeMap = {
		{ERequestType::GET, TEXT("GET")}
		,{ERequestType::POST, TEXT("POST")}
	};	


	// json 데이터 변환 시킬 res 함수 연결용 딜리게이트
	FResponseDelegate tempDel;

	FResponseDelegate signupDel;

	FResponseDelegate loginDel;

	FResponseDelegate tempLoginAuthDel;

public:
#pragma region 사용 함수 연결용 딜리게이트 단
	// res 구조체 데이터 사용할 함수 연결용 딜리게이트
	FResSimpleDelegate tempLoginAuthUseDel;

#pragma endregion

protected:
	// 서버에 요청
	// 구조체 데이터 있음 == 대부분 post 중 일부
	template<typename InStructType>
	void ReqData(FResponseDelegate resDel, const InStructType& structData, const FString& url = "", bool useDefaultURL = true, ERequestType type = ERequestType::POST)
	{
		// 구조체 -> json 화
		FString jsonData;
		FJsonObjectConverter::UStructToJsonObjectString(structData, jsonData, 0, 0);

		ReqData(resDel, jsonData, url, useDefaultURL, type);
	}

	// 구조체 데이터 없음 == get, post 일부
	void ReqData(FResponseDelegate resDel, const FString& jsonData = TEXT(""),  const FString& url = "", bool useDefaultURL = true, ERequestType type = ERequestType::POST);

	// solved 테스트용 데이터 주고 받기
	UFUNCTION(BlueprintCallable)
	void ReqTempCallback();

	UFUNCTION()
	void ResTempCallback(const FString& jsonData, bool isSuccess);

	UFUNCTION()
	void ResSignup(const FString &jsonData, bool isSuccess);

	UFUNCTION()
	void ResLogin(const FString &jsonData, bool isSuccess);

	UFUNCTION()
	void ResLoginAuth(const FString &jsonData, bool isSuccess);

    public:
	template<typename InStructType>
	void RequestToServer(EJsonType type, const InStructType& structData)
	{
		// 구조체 데이터 json 변환
		FString jsonData;
		FJsonObjectConverter::UStructToJsonObjectString(structData, jsonData, 0, 0);

		// req2server
		RequestToServer(type, jsonData);
	}

	void RequestToServer(EJsonType type, const FString &sendJsonData = TEXT(""));
};
