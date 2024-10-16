// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KHS/K_WidgetBase.h"
#include "K_LoadingWidget.generated.h"

/**
 * 
 */
UCLASS()
class MTVS_AIRJET_FINAL_API UK_LoadingWidget : public UK_WidgetBase
{
	GENERATED_BODY()

public:

	virtual bool Initialize(); 
	virtual void SetUI() override;
	virtual void RemoveUI() override;

    // UI 바인딩 구간 ==============================================================================
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UWidgetSwitcher* MenuSwitcher; // 각 메뉴로 전환시킬 수 있는 Menu Switcher

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UWidget* CommonLoadingMenu; // 로딩 메뉴 위젯

	UPROPERTY(EditDefaultsOnly, meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* ShowLoadingAnim; // 로딩 메뉴 Show 애니메이션

	UPROPERTY(EditDefaultsOnly, meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* HideLoadingAnim; // 로딩 메뉴 Hide 애니메이션

};
