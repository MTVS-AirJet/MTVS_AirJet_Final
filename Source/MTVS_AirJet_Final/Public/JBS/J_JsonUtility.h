// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
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
};
