// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "K_PlayerList.generated.h"

/**
 * 
 */
UCLASS()
class MTVS_AIRJET_FINAL_API UK_PlayerList : public UUserWidget
{
	GENERATED_BODY()
	
public:
	
	// 위젯 바인딩 ================================================

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UTextBlock* PlayerNum; // PlayerNum

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UTextBlock* PlayerNickName; // PlayerNickName

	//UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	//class UCheckBox* IsCommander; // 지휘관여부
	
	// Variables    ================================================

	UPROPERTY()
	class UK_StandbyWidget* Parent;

	uint32 Index;

	
	UPROPERTY(BlueprintReadOnly)
	bool Selected = false; // 서버를 선택했는지, 아닌지 선언(Event Graph에서 사용)

	// Functions    ================================================
	void Setup(class UK_StandbyWidget* InParent, uint32 InIndex);

	UFUNCTION()
	bool bIsCommander();


};
