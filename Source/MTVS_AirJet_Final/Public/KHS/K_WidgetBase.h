// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "K_SessionInterface.h"
#include "K_WidgetBase.generated.h"

/**
 * 
 */
UCLASS()
class MTVS_AIRJET_FINAL_API UK_WidgetBase : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetInterface(IK_SessionInterface* Interface); //Interface 할당

	UFUNCTION(BlueprintCallable)
	virtual void SetUI(); // ServerUI 생성 함수(공통)

	UFUNCTION(BlueprintCallable)
	virtual void RemoveUI(); // ServerUI 제거 함수(공통)

	class IK_SessionInterface* SessionInterface; //Interface인스턴스
	
};
