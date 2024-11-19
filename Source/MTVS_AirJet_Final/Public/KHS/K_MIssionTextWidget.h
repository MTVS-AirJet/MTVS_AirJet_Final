// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KHS/K_WidgetBase.h"
#include "K_MIssionTextWidget.generated.h"

struct FAirjetTotalMissionData;
/**
 * 
 */
UCLASS()
class MTVS_AIRJET_FINAL_API UK_MIssionTextWidget : public UK_WidgetBase
{
	GENERATED_BODY()


public:

	virtual void NativeConstruct() override;

	virtual bool Initialize();
	
	class UK_GameInstance* GameInstance; // GameInstance 참조 선언
	
	UPROPERTY(meta = (BindWidget))
	class UWidgetSwitcher* MenuSwitcher; // 각 위젯으로 전환시킬 수 있는 Menu Switcher

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* ShowMissionUIAnim;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* HideMissionUIAnim;
	
	UPROPERTY(meta = (BindWidget))
	class UWidget* MissionText;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Mission_txt_helper;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Mission_txt_acting;

	//UI Visible끄는 함수
	void SetInvisible();
	//FAirjetTotalMissionData 구조체를 전달받아 TextBlock에 SetText하는 함수
	void SetMissionText(FAirjetTotalMissionData* MissionData);
	//ActingText 애니메이션 함수
	void UpdateDisplayedText();
	
	FString DisplayedText; //현재까지 표시된 텍스트들
	FString FullText; //파싱한 전체 텍스트
	int32 CurrentCharIndex; //표시텍스트 인덱스
	FTimerHandle TextDisplayTimerHandle; //텍스트 애니메이션 핸들
	FTimerHandle InvisibleTimerHandle;
};
