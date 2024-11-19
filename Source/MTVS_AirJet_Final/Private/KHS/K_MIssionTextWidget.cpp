// Fill out your copyright notice in the Description page of Project Settings.


#include "KHS/K_MIssionTextWidget.h"

#include "Components/TextBlock.h"
#include "KHS/K_GameInstance.h"
#include "Kismet/GameplayStatics.h"

void UK_MIssionTextWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// GameInstance 가져오기
	GameInstance = Cast<UK_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (!GameInstance)
	{
		UE_LOG(LogTemp , Error , TEXT("GameInstance is null in UK_ServerWidget"));
	}

	SetInvisible(); //처음엔 숨기기
}

bool UK_MIssionTextWidget::Initialize()
{
	Super::Initialize();
	return true;
}

void UK_MIssionTextWidget::SetInvisible()
{
	PlayAnimation(HideMissionUIAnim);
	MissionText->SetVisibility(ESlateVisibility::Hidden);
	GetWorld()->GetTimerManager().ClearTimer(InvisibleTimerHandle);
}

//FAirjetTotalMissionData 구조체를 전달받아 TextBlock에 SetText하는 함수
void UK_MIssionTextWidget::SetMissionText(FAirjetTotalMissionData* MissionData)
{
	//Visibility 켜기
	MissionText->SetVisibility(ESlateVisibility::Visible);
	//애니메이션 재생
    PlayAnimation(ShowMissionUIAnim);
	//SetText
	//Mission_txt_helper->SetText(FText::FromString(MissionData->MissionHelper));
	Mission_txt_acting->SetText(FText::FromString(MissionData->MissionActing));

	if(Mission_txt_helper)
	{
		DisplayedText = "";  // 현재까지 표시된 텍스트를 빈 문자열로 초기화
		FullText = MissionData->MissionHelper;  // 전체 텍스트 저장
		CurrentCharIndex = 0;  // 현재 문자 인덱스 초기화

		// 타이머 설정 (0.1초 간격으로 UpdateDisplayedText 함수 호출)
		GetWorld()->GetTimerManager().SetTimer(TextDisplayTimerHandle, this, &UK_MIssionTextWidget::UpdateDisplayedText, 0.06f, true);
		UE_LOG(LogTemp, Warning, TEXT("Starting text display animation..."));
	}
	//5초 이후에 Visibility 끄기
	GetWorld()->GetTimerManager().SetTimer(InvisibleTimerHandle, this, &UK_MIssionTextWidget::SetInvisible, 8.f, false);
}

void UK_MIssionTextWidget::UpdateDisplayedText()
{
	if (CurrentCharIndex < FullText.Len())
	{
		// 한 글자씩 추가하여 DisplayedText에 저장
		DisplayedText.AppendChar(FullText[CurrentCharIndex]);
		CurrentCharIndex++;

		// 텍스트 블럭에 업데이트
		//UTextBlock* AIChatText = Cast<UTextBlock>(GetWidgetFromName(TEXT("Mission_txt_acting")));
		if (Mission_txt_helper)
		{
			Mission_txt_helper->SetText(FText::FromString(DisplayedText));
		}
	}
	else
	{
		// 모든 텍스트가 출력되었으면 타이머 중지
		GetWorld()->GetTimerManager().ClearTimer(TextDisplayTimerHandle);
		UE_LOG(LogTemp, Warning, TEXT("Finished text display animation."));
	}
}




