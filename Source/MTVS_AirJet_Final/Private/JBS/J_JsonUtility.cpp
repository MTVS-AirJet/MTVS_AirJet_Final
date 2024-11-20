// Fill out your copyright notice in the Description page of Project Settings.


#include "JBS/J_JsonUtility.h"
#include "Containers/Map.h"
#include "Containers/UnrealString.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "JBS/J_Utility.h"
#include "Misc/Base64.h"
#include "Json.h"
#include "JsonObjectConverter.h"
#include "Sound/SoundWave.h"
#include "JsonUtilities.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "Sound/SoundWaveProcedural.h"
#include "Templates/SharedPointer.h"
#include "Kismet/GameplayStatics.h"
#include <JBS/J_GameInstance.h>
#include <Engine/World.h>

#pragma region 손수 json convert
FString UJ_JsonUtility::JsonParse(const FString &jsonData)
{
    // 리더기
    TSharedRef<TJsonReader<TCHAR>> reader = TJsonReaderFactory<TCHAR>::Create(jsonData);
    // 담을 변수 선언
    TSharedPtr<FJsonObject> result = MakeShareable(new FJsonObject());
    // 해석
    FString returnStr = TEXT("");
    if(FJsonSerializer::Deserialize(reader, result))
    {
        // 만약 json 배열이 들어온다면 내부를 배열로 가져옴
        // TArray<TSharedPtr<FJsonValue>> parseDataList = result->GetArrayField(TEXT("items"));
        // for(auto data : parseDataList)
        // {
        //     FString bookName = data->AsObject()->GetStringField(TEXT("bk_nm"));
        //     FString authName = data->AsObject()->GetStringField(TEXT("aut_nm"));
        //     returnStr.Append(FString::Printf(TEXT("%s %s"), *bookName, *authName));
        // }
    }

    return returnStr;
}

FTempJson UJ_JsonUtility::JsonParseTemp(const FString& jsonData)
{
    FTempJson jsonAry;

    FJsonObjectConverter::JsonObjectStringToUStruct<FTempJson>(jsonData, &jsonAry,0,0);

    return jsonAry;
}

FTempJsonAry UJ_JsonUtility::JsonParseTempAry(const FString& jsonData)
{
    FTempJsonAry jsonAry;

    FJsonObjectConverter::JsonObjectStringToUStruct<FTempJsonAry>(jsonData, &jsonAry,0,0);

    return jsonAry;
}

FString UJ_JsonUtility::MakeJson(const TMap<FString, FString> sourceData)
{
    TSharedPtr<FJsonObject> jsonObj = MakeShareable(new FJsonObject());
    for(TPair<FString, FString> pair : sourceData)
    {
        jsonObj->SetStringField(pair.Key, pair.Value);
    }
    FString jsonData;
    // 쓰개 만들기
    TSharedRef<TJsonWriter<TCHAR>> writer = TJsonWriterFactory<TCHAR>::Create(&jsonData);
    if(FJsonSerializer::Serialize(jsonObj.ToSharedRef(), writer))
    {

    }

    return jsonData;
}

#pragma endregion

AJ_JsonManager* UJ_JsonUtility::GetJsonManager(const UWorld* world)
{
    auto* jsonManager = Cast<AJ_JsonManager>(
    UGameplayStatics::GetActorOfClass(world, AJ_JsonManager::StaticClass()));
    check(jsonManager);

    return jsonManager;
}

void UJ_JsonUtility::RequestExecute(const UWorld *world, EJsonType type, const FString &sendJsonData, UJ_GameInstance *gameInstance)
{
    // 게임 인스턴스 못 받았으면 가져오기
    if(!gameInstance)
        gameInstance = UJ_Utility::GetJGameInstance(world);
    
    // 2. 요청 시작
    gameInstance->RequestToServerByType(type, sendJsonData);
}

USoundWaveProcedural *UJ_JsonUtility::ConvertBase64WavToSound(const FString& base64Wav)
{
    // 비어있으면 예외처리
    if(base64Wav.IsEmpty()) return nullptr;
    // 반환할 사운드 객체
    auto* voice = NewObject<USoundWaveProcedural>(USoundWaveProcedural::StaticClass());
    // 사운드 데이터
	TArray<uint8> voiceData;
	if(FBase64::Decode(base64Wav, voiceData))
	{
        // GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("4. 디코딩 완료"));
        // 채널, 샘플, 길이
		int16 chl = 0;
		int32 sampleRate = 0;
		int32 dataSize = 0;
		
        // wav 파일 구성 설정
		FMemory::Memcpy(&chl, &voiceData[22], sizeof(int16));   // 채널 수
		FMemory::Memcpy(&sampleRate, &voiceData[24], sizeof(int32));    // 샘플 속도
		FMemory::Memcpy(&dataSize, &voiceData[40], sizeof(int32)); 

		voice->SetSampleRate(sampleRate);
		voice->NumChannels = chl;
		voice->Duration = static_cast<float>(dataSize) / (sampleRate * chl * sizeof(int16));
		voice->bLooping = false;

        // 실제 사운드 데이터 삽입 | wav 헤더 44 이후
		TArray<uint8> pcmData(voiceData.GetData() + 44, voiceData.Num() - 44);
		if(pcmData.Num() > 0)
			voice->QueueAudio(pcmData.GetData(), pcmData.Num());
	}

    return voice;
}