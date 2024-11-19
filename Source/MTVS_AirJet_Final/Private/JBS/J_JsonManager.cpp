// Fill out your copyright notice in the Description page of Project Settings.


#include "JBS/J_JsonManager.h"
#include "Engine/Engine.h"
#include "JBS/J_Utility.h"
#include "Serialization/BulkData.h"
#include "Sound/SoundWave.h"
#include "Sound/SoundCue.h"
#include "Misc/Base64.h"
#include "Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"
#include "Sound/SoundWaveProcedural.h"
#include "UObject/UObjectGlobals.h"
#include <JBS/J_JsonUtility.h>
#include <KHS/K_GameInstance.h>

// Sets default values
AJ_JsonManager::AJ_JsonManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AJ_JsonManager::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AJ_JsonManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(bStopWatch)
		stopWatchTime += DeltaTime;
	
	GEngine->AddOnScreenDebugMessage(-1, -1.f, FColor::Green, FString::Printf(TEXT("스톱워치 : %.2f"), stopWatchTime));
	


}
void AJ_JsonManager::ReqTemp()
{
	FResSimple simple;
	simple.success = true;
	simple.response = TEXT("doremifarondo");

	UJ_JsonUtility::RequestExecute(GetWorld(), EJsonType::TEMP01_CALLBACK, simple);
}

// 0-1. 회원 가입 요청 예시
void AJ_JsonManager::ReqSignup()
{
	// 그냥 요청만 하는 예시
	UJ_JsonUtility::RequestExecute<FSignupReq>(GetWorld(), EJsonType::SIGN_UP, tempSignup);
}

void AJ_JsonManager::ReqLogin()
{
	// auto* gi = UJ_Utility::GetJGameInstance(GetWorld());
	// gi->tempLoginResUseDel.BindLambda([this](const FLoginResponse& resData){
	// 	GEngine->AddOnScreenDebugMessage(-1, 31.f, FColor::Yellow, FString::Printf(TEXT("jsonManager에서 출력됨\n%s"), *resData.ToString()));
	// });


	UJ_JsonUtility::RequestExecute<FLoginReq>(GetWorld(), EJsonType::LOGIN, tempLogin);
}

// 0-2. 요청하고 반응 데이터까지 받아오는 예시
void AJ_JsonManager::ReqTempAuth()
{
	// 게임 인스턴스 가져와서 만들어둔 딜리게이트에 내 함수 바인딩
	auto* gi = UJ_Utility::GetKGameInstance(GetWorld());
	gi->tempLoginAuthUseDel.BindUObject(this, &AJ_JsonManager::OnLoginAuthData);

	// 서버에 요청 시작 -> 1~4 단계를 거쳐 바인드한 함수에 데이터가 들어옴.
	UJ_JsonUtility::RequestExecute<FLoginReq>(GetWorld(), EJsonType::TEMP02_AUTH, tempLogin);
}

void AJ_JsonManager::OnLoginAuthData(const FResSimple &resData)
{
	// GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("asd"));
	GEngine->AddOnScreenDebugMessage(-1, 31.f, FColor::Yellow, FString::Printf(TEXT("jsonManager에서 출력됨\n%s"), *resData.ToString()));
}

// 5. 반응 데이터 처리하는 예시
void AJ_JsonManager::OnLoginData(const FLoginRes &resData)
{
	GEngine->AddOnScreenDebugMessage(-1, 31.f, FColor::Yellow, FString::Printf(TEXT("jsonManager에서 출력됨\n%s"), *resData.ToString()));
}


void AJ_JsonManager::ReqGetMissionData()
{
	FMissionDataReq tempreq(tempReqMapName);

	// 게임 인스턴스 가져와서 만들어둔 딜리게이트에 내 함수 바인딩
	auto* gi = UJ_Utility::GetKGameInstance(GetWorld());
	gi->missionDataResDelegate.BindUObject(this, &AJ_JsonManager::OnGetMissionData);

	// 서버에 요청 시작 -> 1~4 단계를 거쳐 바인드한 함수에 데이터가 들어옴.
	UJ_JsonUtility::RequestExecute(GetWorld(), EJsonType::MISSION_DATA_RECEIVE, tempreq);
}

void AJ_JsonManager::OnGetMissionData(const FMissionDataRes &resData)
{
	GEngine->AddOnScreenDebugMessage(-1, 333.f, FColor::Yellow, FString::Printf(TEXT("jsonManager에서 출력됨\n%s"), *resData.ToString()));

	tempGetMissionData = resData;
}

void AJ_JsonManager::ReqCV()
{
	// 게임 인스턴스 가져와서 만들어둔 딜리게이트에 내 함수 바인딩
	auto* gi = UJ_Utility::GetKGameInstance(GetWorld());
	gi->commanderVoiceResUseDel.BindUObject(this, &AJ_JsonManager::OnGetCVData);

	// 서버에 요청 시작 -> 1~4 단계를 거쳐 바인드한 함수에 데이터가 들어옴.
	UJ_JsonUtility::RequestExecute(GetWorld(), EJsonType::COMMANDER_VOICE, tempCVReq);
}

void AJ_JsonManager::OnGetCVData(const FCommanderVoiceRes &resData)
{
	GEngine->AddOnScreenDebugMessage(-1, 333.f, FColor::Yellow, FString::Printf(TEXT("jsonManager에서 출력됨\n%s"), *resData.voice));

	tempVoice = UJ_JsonUtility::ConvertBase64WavToSound(resData.voice);

	PlayTempVoice();
}

void AJ_JsonManager::ReqAIFeedback()
{
	// 게임 인스턴스 가져와서 만들어둔 딜리게이트에 내 함수 바인딩
	auto* gi = UJ_Utility::GetKGameInstance(GetWorld());
	gi->aiFeedbackResUseDel.BindUObject(this, &AJ_JsonManager::OnGetAIFeedbackData);

	// 서버에 요청 시작 -> 1~4 단계를 거쳐 바인드한 함수에 데이터가 들어옴.
	UJ_JsonUtility::RequestExecute(GetWorld(), EJsonType::AI_FEEDBACK, tempAIFBReq);
	ToggleStopWatch();
}

void AJ_JsonManager::OnGetAIFeedbackData(const FAIFeedbackRes &resData)
{
	GEngine->AddOnScreenDebugMessage(-1, 333.f, FColor::Yellow, FString::Printf(TEXT("jsonManager에서 출력됨\n%s"), *resData.ToString()));
	ToggleStopWatch();
}

void AJ_JsonManager::ReqAllVoice()
{
	// 게임 인스턴스 가져와서 만들어둔 딜리게이트에 내 함수 바인딩
	auto* gi = UJ_Utility::GetKGameInstance(GetWorld());
	gi->allVoiceResUseDel.BindUObject(this, &AJ_JsonManager::OnGetAllVoiceData);

	// 서버에 요청 시작 -> 1~4 단계를 거쳐 바인드한 함수에 데이터가 들어옴.
	UJ_JsonUtility::RequestExecute(GetWorld(), EJsonType::ALL_VOICE);
	ToggleStopWatch();
}

void AJ_JsonManager::OnGetAllVoiceData(const FAllVoiceRes &resData)
{
	FString debugStr = FString::Printf(TEXT("보이스 개수 : %d"), resData.data.Num());

	GEngine->AddOnScreenDebugMessage(-1, 333.f, FColor::Yellow, FString::Printf(TEXT("jsonManager에서 출력됨\n%s"), *debugStr));

	allVoiceData = resData.data;
	ToggleStopWatch();
}
