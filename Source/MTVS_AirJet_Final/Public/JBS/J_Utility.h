// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Templates/SubclassOf.h"
#include "J_Utility.generated.h"

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

// 통신 타입 
UENUM(BlueprintType)
enum class EJsonType : uint8
{
    TEMP01_CALLBACK = 0
    ,SIGN_UP
    ,LOGIN
    ,TEMP02_AUTH
};

#pragma region 웹 통신 용 구조체

#pragma region 요청단
USTRUCT(BlueprintType)
struct FSignupReq
{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
    FString loginId;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
    FString password;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
    FString confirmPassword;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
    FString nickname;

    FSignupReq() : loginId(TEXT("미설정")), password(TEXT("미설정")), confirmPassword(TEXT("미설정")), nickname(TEXT("미설정")) {}

    FSignupReq(FString loginId ,FString password ,FString confirmPassword ,FString nickname) 
        : loginId(loginId)
        ,password(password)
        ,confirmPassword(confirmPassword)
        ,nickname(nickname) {}
};

USTRUCT(BlueprintType)
struct FLoginReq
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
    FString loginId;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
    FString password;

    FLoginReq() : loginId(TEXT("미설정")), password(TEXT("미설정")) {}

    FLoginReq(FString loginId ,FString password) 
        : loginId(loginId)
        ,password(password) {}
};

#pragma endregion

#pragma region 반환단

USTRUCT(BlueprintType)
struct FResSimple
{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
    bool success;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
    FString response;

    FResSimple() : success(false), response(TEXT("미설정")) {}

    FString ToString() const
    {
        FString str = FString::Printf(TEXT("성공 여부 : %s\n응답 결과 데이터 : %s")
            , this->success ? TEXT("TRUE") : TEXT("FALSE")
            , *this->response);

        return str;
    }
};

USTRUCT(BlueprintType)
struct FLoginRes
{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
    bool success;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
    FString response;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
    FString loginId;

    FLoginRes() : success(false), response(TEXT("미설정")) {}

    FString ToString() const
    {
        FString str = FString::Printf(TEXT("성공 여부 : %s\n응답 결과 데이터 : %s\n로그인 id : %s")
            , this->success ? TEXT("TRUE") : TEXT("FALSE")
            , *this->response
            , *this->loginId);

        return str;
    }
};

#pragma endregion

#pragma endregion

#pragma region 미션 관련

UENUM(BlueprintType)
enum class EPlayerRole : uint8
{
    COMMANDER = 0
    ,PILOT = 1
};

USTRUCT(BlueprintType)
struct FMissionPlayerSpawnPoints
{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
    FName spawnPointTag;
    // 스폰 포인트 맵
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Objects")
    TMap<EPlayerRole, class AJ_MissionSpawnPointActor*> spawnPointMap;

    // 스폰 포인트 기본 위치 맵
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
    TMap<EPlayerRole, FTransform> spawnPointDefaultPos;

    // 스폰 포인트 프리팹
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Classes")
    TSubclassOf<class AJ_MissionSpawnPointActor> spawnPointPrefab;
    // TSubclassOf<AActor> spawnPointPrefab;
};

#pragma endregion

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
class MTVS_AIRJET_FINAL_API UJ_Utility : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
    static class UJ_GameInstance *GetJGameInstance(const UWorld* world);

    static class UK_GameInstance* GetKGameInstance(const UWorld* world);

    static class AJ_MissionGamemode* GetMissionGamemode(const UWorld* world);
};
