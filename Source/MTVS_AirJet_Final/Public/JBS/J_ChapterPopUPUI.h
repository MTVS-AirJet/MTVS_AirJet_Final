// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "JBS/J_Utility.h"
#include "J_ChapterPopUPUI.generated.h"

// 딜리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPopUPActiveDelegate);


UCLASS()
class MTVS_AIRJET_FINAL_API UJ_ChapterPopUPUI : public UUserWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|UI", meta = (BindWidget))
	class UImage* portraitImg;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|UI", meta = (BindWidget))
	class URichTextBlock* popupText;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|UI", meta = (BindWidget))
	class UImage* popupTextImg;

	// 텍스트 스타일
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
	ETextStyle textStyle;

	// 텍스트 이미지 맵( 미션 절차, 텍스쳐 )
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Classes")
	TMap<EMissionProcess, FSlateBrush> textImgMap;

	// 초상화 이미지 맵( 미션 절차, 텍스쳐 )
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Classes")
	TMap<EMissionProcess, FSlateBrush> portraitImgMap;

	

	// 비활성화 딜레이 시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
	float deActiveDelay = 5.f;

public:
	// 비활성화 타이머
	FTimerHandle deactiveTimer;

#pragma region 딜리게이트 단
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Values")
	FPopUPActiveDelegate activeDel;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Values")
	FPopUPActiveDelegate deactiveDel;

#pragma endregion

protected:
	// 활성 애니메이션
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void PlayActiveAnim();

public:
	// 비활성 애니메이션
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void PlayDeactiveAnim();

	// 미션 절차를 받아서 dt에서 텍스트 가져와서 설정
	UFUNCTION(BlueprintCallable)
	void SetPopupText(const EMissionProcess& mpIdx, const FString& normalStr, const float& newDelayTime = -1.f);

	UFUNCTION(BlueprintCallable)
	void SetActive(bool value);
};
