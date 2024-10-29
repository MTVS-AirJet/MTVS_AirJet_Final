// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "JBS/J_Utility.h"
#include "J_ObjectiveUI.generated.h"

/**
 * 
 */
UCLASS()
class MTVS_AIRJET_FINAL_API UJ_ObjectiveUI : public UUserWidget
{
	GENERATED_BODY()
protected:
	// 위젯 스위쳐 | 목표 UI, 결산 UI
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|UI", meta = (BindWidget))	
	class UWidgetSwitcher* MissionUI_Switcher;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|UI", meta = (BindWidget))
	class UTextBlock* objHeaderText;

	// @@ 조건 2개 이상 (편대비행) 같은 경우를 위해 따로 subUI 구조 필요
	// UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|UI", meta = (BindWidget))
	// class UTextBlock* objBodyText;

	// 서브 조건 리스트 VBox
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|UI", meta = (BindWidget))
	class UVerticalBox* objBodyElementVBox;
	


	// 기본 text color
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
	FSlateColor defaultTextColor;
	// 완료 text color
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
	FSlateColor successTextColor;
	// 실패 text color
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
	FSlateColor failTextColor;

public:
	// 결산 UI
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|UI", meta = (BindWidget))
	class UJ_MissionCompleteUI* missionCompleteUI;

protected:
	// @@ 세부 조건 UI 생성


	// 전술명령 시작 UMG
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void PlayObjStartAnim();

	// 전술명령 완료 UMG
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void PlayObjEndAnim();


public:
	// 전술명령 텍스트 설정
    void SetObjUI(FObjUIData data);
    // 전술명령 헤더 텍스트 설정
    void SetObjHeaderText(const FString &value);
    // 전술명령 세부 조건 텍스트 설정
    void SetObjBodyText(TArray<FString> valueAry);
	// 전술명령 시작 처리
    void StartObjUI();
    // 세부 조건 완료 처리
    void EndSubObjUI(int idx, bool isSuccess);
    // 전술명령 완료 처리
    void EndObjUI(bool isSuccess);


	// 결산 UI 표시
	void ActiveResultUI() {}
};
