// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "L_WaitingForStart.generated.h"

/**
 * 
 */
UCLASS()
class MTVS_AIRJET_FINAL_API UL_WaitingForStart : public UUserWidget
{
	GENERATED_BODY()

private:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	UPROPERTY()
	class AK_GameState* gs;
	UPROPERTY()
	class UK_GameInstance* gi;
	int32 MaxCnt = 0;

public:
	UPROPERTY(meta = (BindWidget) , Category="MemberCound" , EditDefaultsOnly)
	class UTextBlock* TxtReadyCnt;

	int32 CurrentCnt = 0;

	void SetMem(const int32& newMem);
};
