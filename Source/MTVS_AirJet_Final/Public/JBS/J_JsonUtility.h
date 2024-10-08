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
    static FString JsonParse(const FString &jsonData);
    static FString MakeJson(const TMap<FString, FString> sourceData);

    static FTempJson JsonParseTemp(const FString& jsonData);
    static FTempJsonAry JsonParseTempAry(const FString& jsonData);

    // 레벨의 jsonManager 찾기
    static class AJ_JsonManager* GetJsonManager(const UWorld* world);

    // 게임 인스턴스 가져오기

    // json 요청
    template<typename InStructType>
    static void RequestExecute(const UWorld* world, EJsonType type, const InStructType& structData, UK_GameInstance* gameInstance = nullptr)
    {
        // 없으면 찾기
        if(gameInstance == nullptr)
            gameInstance = UJ_Utility::GetKGameInstance(world);
        
        // 요청 시작
        gameInstance->RequestToServer<InStructType>(type, structData);
    }
};
