// Fill out your copyright notice in the Description page of Project Settings.


#include "JBS/J_JsonUtility.h"
#include "Containers/Map.h"
#include "Containers/UnrealString.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "Json.h"
#include "JsonObjectConverter.h"
#include "JsonUtilities.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "Templates/SharedPointer.h"

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