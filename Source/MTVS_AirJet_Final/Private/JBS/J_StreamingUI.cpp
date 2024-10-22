// Fill out your copyright notice in the Description page of Project Settings.


#include "JBS/J_StreamingUI.h"
#include "Components/Image.h"
#include "Components/SlateWrapperTypes.h"
#include "Engine/Engine.h"
#include "JBS/J_BaseMissionPawn.h"
#include "KHS/K_StreamingActor.h"
#include "Kismet/GameplayStatics.h"

void UJ_StreamingUI::NativeConstruct()
{
    // 레벨에 배치된 ScreenActor를 찾음
    auto* tempSA = Cast<AK_StreamingActor>(
        UGameplayStatics::GetActorOfClass(GetWorld(), AK_StreamingActor::StaticClass()));
    if(tempSA)
    {
        //해당 액터로 뭔가 하기
        ScreenActor = tempSA;
    }

	ButtonLookSharingScreen->OnClicked.AddDynamic(this , &UJ_StreamingUI::OnButtonLookSharingScreen);
	ButtonWindowScreen->OnClicked.AddDynamic(this , &UK_StreamingUI::OnButtonWindowScreen);
	ButtonLookSharingScreen->SetVisibility(ESlateVisibility::Hidden);
	ImageSharingScreen->SetVisibility(ESlateVisibility::Hidden);

}

void UJ_StreamingUI::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    this->TextWindowScreen->SetText(FText::FromString(TEXT("Screen Share")));
}

AJ_BaseMissionPawn* UJ_StreamingUI::GetGameObject()
{
    if(!gameObject)
    {
        gameObject = GetOwningPlayerPawn<AJ_BaseMissionPawn>();
    }

    return gameObject;
}

void UJ_StreamingUI::OnButtonWindowScreen()
{
    Super::OnButtonWindowScreen();

    // GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("3. 공유 버튼 클릭"));
}

void UJ_StreamingUI::OnButtonLookSharingScreen()
{
    Super::OnButtonLookSharingScreen();

    // GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("6. 화면 공유 시작"));

    ButtonLookSharingScreen->SetVisibility(ESlateVisibility::Visible);

}

void UJ_StreamingUI::SetUserID(FString ID, const bool &bAddPlayer)
{
    Super::SetUserID(ID, bAddPlayer);

    GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("4. 유저 아이디 설정"));
}