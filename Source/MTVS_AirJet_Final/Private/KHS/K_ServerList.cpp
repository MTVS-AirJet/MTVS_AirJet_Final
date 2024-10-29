// Fill out your copyright notice in the Description page of Project Settings.


#include "KHS/K_ServerList.h"
#include "KHS/K_ServerWidget.h"
#include "KHS/K_GameInstance.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include <MTVS_AirJet_Final.h>

void UK_ServerList::Setup(class UK_ServerWidget* InParent , int InIndex)
{
	Parent = InParent;
	Index = InIndex;
	RowButton->OnClicked.AddDynamic(this , &UK_ServerList::OnClicked);
}


void UK_ServerList::OnClicked()
{
	Parent->SelecetIndex(Index);

    UE_LOG(LogTemp , Warning , TEXT("Hovered on ServerList index: %d") , Index);

    // 세션 인덱스에 맞는 ServerData를 가져옴
    if ( Parent && Parent->GameInstance )
    {
        //해당 인덱스 Iterator를 받고, Index검색결과가 있다면(.end()가 아니라면)
        auto It = Parent->GameInstance->ServerDataList.find(Index);
        if ( It != Parent->GameInstance->ServerDataList.end() )
        {
            FString ServerData = It->second;  // iterator Pair에서 FString멤버(Value) 값을 가져옴

            // 필요한 정보만 추출 (예시: 제작자명과 맵 이름)
            TArray<FString> ParsedData;
            ServerData.ParseIntoArray(ParsedData , TEXT("|"));

            if ( ParsedData.Num() >= 4 )
            {
                UE_LOG(LogTemp , Warning , TEXT("Setting Producer: %s, MapName: %s") , *ParsedData[0] , *ParsedData[3]);

                // 제작자와 맵 이름을 Parent의 텍스트 블록에 설정
                Parent->ServerMenu_txt_Producer->SetText(FText::FromString(ParsedData[0]));
                Parent->ServerMenu_txt_MapName->SetText(FText::FromString(ParsedData[3]));

                //이제 Img정보도 다시 불러와서 세팅하는 작업을 해야함
                FMapInfoRequest mapName = ParsedData[3];
                Parent->ReqSessionInfo(mapName);

            }
        }
    }
}