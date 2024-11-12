// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include <JBS/J_Utility.h>
#include <JBS/J_JsonManager.h>
#include <KHS/K_GameInstance.h>
#include "J_JsonUtility.generated.h"

/**
 * 
 */




UCLASS()
class MTVS_AIRJET_FINAL_API UJ_JsonUtility : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
    // XXX 필요 없음
    static FString JsonParse(const FString &jsonData);
    static FString MakeJson(const TMap<FString, FString> sourceData);

    static FTempJson JsonParseTemp(const FString& jsonData);
    static FTempJsonAry JsonParseTempAry(const FString& jsonData);

    // 레벨의 jsonManager 찾기
    static class AJ_JsonManager* GetJsonManager(const UWorld* world);
    // /

    // 템플릿 함수라 인라인 함수로 작성함.
    // 1-1. 웹 통신 요청 : getworld(), 요청 타입, 구조체 데이터, 게임 인스턴스(생략가능)
    template<typename InStructType>
    static void RequestExecute(const UWorld* world
                            , EJsonType type
                            , const InStructType& structData
                            , UJ_GameInstance* gameInstance = nullptr)
    {
        // 구조체 데이터 json string 데이터로 변환
        FString jsonData;
		FJsonObjectConverter::UStructToJsonObjectString(structData, jsonData, 0, 0);
        
        // 아래 함수로 요청 시작
        RequestExecute(world, type, jsonData, gameInstance);
    }

    // 1-2. 웹 통신 요청 : getworld(), 요청 타입, json string 데이터(생략가능), 게임 인스턴스(생략가능)
    static void RequestExecute(const UWorld *world
                            , EJsonType type
                            , const FString &sendJsonData = TEXT("")
                            , UJ_GameInstance *gameInstance = nullptr);
    
    // base64 인코딩 한 wav 파일을 USoundWaveProcedural 로 변환
    static USoundWaveProcedural *ConvertBase64WavToSound(const FString& base64Wav);
};
