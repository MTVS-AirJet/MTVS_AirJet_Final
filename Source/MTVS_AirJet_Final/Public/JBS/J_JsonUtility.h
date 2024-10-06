// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "J_JsonUtility.generated.h"

/**
 * 
 */
// 전송 타입
UENUM(BlueprintType)
enum class ERequestType : uint8
{
    GET = 0
    ,POST = 1
};

// @@ 테스트 용
USTRUCT(BlueprintType)
struct FTempJson
{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
    int userId;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
    int id;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
    FString title;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
    FString body;

    FTempJson() : userId(-1), id(-1), title(TEXT("미정")), body(TEXT("없음")) {}

    FString ToString() const
    {
        return FString::Printf(TEXT("userid : %d, id : %d, title : %s, body : %s\n"), userId, id, *title, *body);
    }
};

USTRUCT(BlueprintType)
struct FTempJsonAry
{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
    TArray<FTempJson> tempJsons;
};

UCLASS()
class MTVS_AIRJET_FINAL_API UJ_JsonUtility : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
    static FString JsonParse(const FString &jsonData);
    static FString MakeJson(const TMap<FString, FString> sourceData);

    static FTempJson JsonParseTemp(const FString& jsonData);
    static FTempJsonAry JsonParseTempAry(const FString& jsonData);
};
