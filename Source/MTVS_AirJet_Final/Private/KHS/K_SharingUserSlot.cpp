// Fill out your copyright notice in the Description page of Project Settings.


#include "KHS/K_SharingUserSlot.h"
#include "KHS/K_StreamingActor.h"
#include "KHS/K_StreamingUI.h"
#include "KHS/K_PlayerController.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerController.h"

void UK_SharingUserSlot::NativeConstruct()
{
    // 레벨에 배치된 ScreenActor를 찾음
    for ( TActorIterator<AK_StreamingActor> It(GetWorld() , AK_StreamingActor::StaticClass()); It; ++It )
    {
        StreamingActor = *It;
        break;
    }

    auto* pc = Cast<AK_PlayerController>(GetWorld()->GetFirstPlayerController());
    StreamingUI = pc->StreamingUI;

    // 버튼 클릭 시 호출할 함수 바인딩
    if ( UserIDButton )
    {
        UserIDButton->OnClicked.AddDynamic(this , &UK_SharingUserSlot::OnUserIDButtonClicked);
    }
}

// UserID를 설정하는 함수
void UK_SharingUserSlot::SetUserID(FString ID)
{
    // UserID를 설정하고 UI를 업데이트
    CurrentUserID = ID;

    if ( UserIDText )
    {
        UserIDText->SetText(FText::FromString(CurrentUserID));
    }
}

// 사용자 ID 버튼 클릭 시 호출될 함수
void UK_SharingUserSlot::OnUserIDButtonClicked()
{
   // 버튼 클릭 시 발생하는 이벤트, 다른 스트리머의 화면을 볼 수 있어야함
   //아이디 전환
   //ScreenActor->ChangeLookSharingScreen();
    if ( StreamingUI )
        StreamingUI->OnButtonLookSharingScreen();
}
