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

	// 전술 명령 UI
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|UI", meta = (BindWidget))
	class UJ_ObjectiveTextUI* objectiveTextUI;
		public:
	__declspec(property(get = GetObjTextUI, put = SetObjTextUI)) class UJ_ObjectiveTextUI* OBJ_TEXT_UI;
	class UJ_ObjectiveTextUI* GetObjTextUI()
	{
		return objectiveTextUI;
	}
	void SetObjTextUI(class UJ_ObjectiveTextUI* value)
	{
		objectiveTextUI = value;
	}
		protected:

	// 상세 ui
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|UI", meta = (BindWidget))
	class UJ_DetailUI* detailTextUI;
		public:
	__declspec(property(get = GetDetailTextUI, put = SetDetailTextUI)) class UJ_DetailUI* DETAIL_TEXT_UI;
	class UJ_DetailUI* GetDetailTextUI()
	{
		return detailTextUI;
	}
	void SetDetailTextUI(class UJ_DetailUI* value)
	{
		detailTextUI = value;
	}
		protected:

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
	// 전술명령 시작 UMG
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void PlayObjStartAnim(class UJ_ObjectiveTextUI* objTextUI);

	// 전술명령 완료 UMG
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void PlayObjEndAnim(class UJ_ObjectiveTextUI* objTextUI);


public:
	// 전술명령 텍스트 설정
    void SetObjUI(FTextUIData data, bool isInit = false);
	void SetObjUI(TArray<FTextUIData>& data, bool isInit = false);
	// 전술명령 시작 처리
    void StartObjUI();
    // 세부 조건 완료 처리
    void EndSubObjUI(int idx, bool isSuccess);
    // 전술명령 완료 처리
    void EndObjUI(bool isSuccess);
	// 텍스트 색상 초기화
    void ResetTextColor();

    // 결산 UI 표시
    void ActiveResultUI(const TArray<FObjectiveData>& resultObjData);

};
