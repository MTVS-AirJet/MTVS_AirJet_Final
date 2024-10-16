// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Templates/SubclassOf.h"
#include "K_JsonParseLib.generated.h"

// 전송요청 타입(GET, POST)
UENUM(BlueprintType)
enum class EReqType : uint8
{
    GET = 0
    ,POST = 1
};

// JSON 이벤트 타입(회원가입, 로그인, 맵정보)
UENUM(BlueprintType)
enum class EEventType : uint8
{
    REGISTER = 0
    ,LOGIN = 1
    ,MAPINFO = 2
    ,MAPSET = 3
};

#pragma region 백엔드 통신용 STRUCT

#pragma region REQUEST STRUCT

//회원가입 사용 구조체
USTRUCT(BlueprintType)
struct FRegisterRequest
{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Register | Members")
    FString loginId; //ID
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Register | Members")
    FString password; //PW
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Register | Members")
    FString confirmPassword; //PW Check
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Register | Members")
    FString nickname; //NickName

    FRegisterRequest() : loginId(TEXT("미설정")), password(TEXT("미설정")), confirmPassword(TEXT("미설정")), nickname(TEXT("미설정")) {}

    FRegisterRequest(FString loginId ,FString password ,FString confirmPassword ,FString nickname)
        : loginId(loginId)
        ,password(password)
        ,confirmPassword(confirmPassword)
        ,nickname(nickname) {}
};

//로그인 사용 구조체
USTRUCT(BlueprintType)
struct FLoginRequest
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Login | Members")
    FString loginId; //ID
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Login | Members")
    FString password; //PW

    FLoginRequest() : loginId(TEXT("미설정")), password(TEXT("미설정")) {}

    FLoginRequest(FString loginId ,FString password)
        : loginId(loginId)
        ,password(password) {}
};

//맵정보 요청 구조체
USTRUCT(BlueprintType)
struct FMapInfoRequest
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
    FString mapName; //MapName
    
    FMapInfoRequest() : mapName(TEXT("미설정")) {}

    FMapInfoRequest(FString mapName)
        : mapName(mapName){}
};

#pragma endregion

#pragma region RESPONSE STRUCT

//회원가입 사용 구조체
USTRUCT(BlueprintType)
struct FRegisterResponse
{
    GENERATED_BODY()
    
    // 성공 여부
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Response | Members")
    bool success;
    // 응답 메시지
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Response | Members")
    FString response;

    FString ResponseToString() const
    {
        FString str = FString::Printf(TEXT("성공 여부 : %s\n응답 결과 데이터 : %s")
            , this->success ? TEXT("TRUE") : TEXT("FALSE")
            , *this->response);

        return str;
    }
};

//로그인 사용 구조체
USTRUCT(BlueprintType)
struct FLoginResponse : public FRegisterResponse
{
    GENERATED_BODY()

    // 로그인 아이디
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Response | Members")
    FString loginId;

    FString ResponseToString() const
    {
        FString str = FString::Printf(TEXT("성공 여부 : %s\n응답 결과 데이터 : %s\n로그인 id : %s")
            , this->success ? TEXT("TRUE") : TEXT("FALSE")
            , *this->response
            , *this->loginId);

        return str;
    }
};

//맵정보 사용 구조체
USTRUCT(BlueprintType)
struct FMapInfoResponse
{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Response | Members")
    FString producer; // 맵 제작자
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Response | Members")
    float latitude; // 위도
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Response | Members")
    float longitude; // 경도
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Response | Members")
    FString mapName; // 맵 이름
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Response | Members | Image")
    FString mapImage; // 썸네일 이미지 바이트 배열
    //TArray<uint8> mapImage;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Response | Members")
    int pinNo; // 핀 Num
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Response | Members")
    int missionX; // 핀 X값
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Response | Members")
    int missionY; // 핀 Y값
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Response | Members")
    int startPointX; // 시작점 X값
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Response | Members")
    int startPointY; // 시작점 Y값
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Response | Members")
    int commandNo; // 지휘명령 Num


    FString ResponseToString() const
    {
        FString str = FString::Printf(
        TEXT("맵 제작자 : %s\n위도 : %.2f , 경도 : %.2f\n맵 이름 : %s\n썸네일 이미지 존재 : %s\n시작 지점 : %d,%d\n핀Num : %d\n지휘명령Num : %d")
        , *producer
        , latitude
        , longitude
        , *mapName
        , mapImage.IsEmpty() ? TEXT("FALSE") : TEXT("TRUE")
        , startPointX, startPointY
        , pinNo
        , commandNo
        );

        return str;
    }
};

#pragma endregion

#pragma endregion

/**
 * 
 */
UCLASS()
class MTVS_AIRJET_FINAL_API UK_JsonParseLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
    static class UJ_GameInstance* GetJGameInstance(const UWorld* world);
    // 게임 인스턴스 가져오기
    static class UK_GameInstance* GetKGameInstance(const UWorld* world);
    // 미션 게임모드 가져오기
    static class AJ_MissionGamemode* GetMissionGamemode(const UWorld* world);
    // 게임 스테이트 가져오기
    static class AK_GameState* GetKGameState(const UWorld* world);
    // 미션맵 로컬 플레이어 가져오기
    static class AJ_BaseMissionPawn* GetBaseMissionPawn(const UWorld* world , int32 playerIdx = 0);

};
