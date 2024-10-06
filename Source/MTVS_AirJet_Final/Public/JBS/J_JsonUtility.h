// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include <JBS/J_Utility.h>
#include <JBS/J_JsonManager.h>
#include "J_JsonUtility.generated.h"

/**
 * 
 */




UCLASS()
class MTVS_AIRJET_FINAL_API UJ_JsonUtility : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
    static FString JsonParse(const FString &jsonData);
    static FString MakeJson(const TMap<FString, FString> sourceData);

    static FTempJson JsonParseTemp(const FString& jsonData);
    static FTempJsonAry JsonParseTempAry(const FString& jsonData);

    // 레벨의 jsonManager 찾기
    static class AJ_JsonManager* GetJsonManager(const UWorld* world);

    // json 요청
    template<typename InStructType>
    static void RequestExecute(const UWorld* world, EJsonType type, const InStructType& structData, AJ_JsonManager* manager = nullptr)
    {
        // 없으면 찾기
        if(manager == nullptr)
            manager = UJ_JsonUtility::GetJsonManager(world);
        
        // 요청 시작
        manager->RequestToServer<InStructType>(type, structData);
    }
};
