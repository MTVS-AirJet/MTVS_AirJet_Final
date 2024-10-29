// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "J_PilotViewTabletUI.generated.h"

/**
 * 
 */
UCLASS()
class MTVS_AIRJET_FINAL_API UJ_PilotViewTabletUI : public UUserWidget
{
	GENERATED_BODY()
	// 그니깐 플레이어 선택 ui를
	// 플레이어 수에 따라
	// 2개 단위로 row가 추가되는 2xn 형태로 하고싶은거고
	// 그러면 한 row를 sub ui 단위로 해서
	// 새로 추가하면 새 row 추가 및 왼쪽 활성화
	// 더 추가하면 오른쪽 활성화
	// 더 추가하면 새 row 추가...
	// 로 가고 버튼에 playerId 내장 시켜서 누르면 해당 값 매개변수로 해서 태블릿에 시청하라고 가도록 딜리게이트 설정 필요
	// 뎁스 하나 줄이자
protected:
#pragma region UI 위젯 클래스 단
	// 위젯 스위치 | 0 = PV, 1 = PVS
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|UI", meta = (BindWidget))
	class UWidgetSwitcher* PilotViewWidgetSwitcher;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|UI", meta = (BindWidget))	
	class UButton* PV_ScreenBtn;

	// 플레이어 슬롯 root 컬럼
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|UI", meta = (BindWidget))
	class UVerticalBox* PVS_SlotColumn;
	

#pragma endregion

	// FIXME 테스트용으로 사용할 임시 플레이어 id 배열
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Debug")
	TArray<FString> tempPlayerIds;

	// 플레이어 슬롯 프리팹
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Classes")
	TSubclassOf<class UJ_PlayerSlotUI> playerSlotUIPrefab;

	// 최말단 horizontal box
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Component")
	class UHorizontalBox* curHBox = nullptr;

	// hori box 가 가질 수 있는 요소 개수 == x*y 중 x 결정
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
	int playerSlotRowSize = 2;
	

public:

protected:
	// ui 생성시 설정
    virtual void NativeConstruct() override;

	// 플레이어 시점 선택 ui로 이동
	UFUNCTION(BlueprintCallable)
    void OnClickGoSelectScreen();

    // 플레이어 슬롯 UI 추가
    void AddPlayerSlot(const FString& playerId);

    // 플레이어 슬롯 row horizontal box 추가
    class UHorizontalBox *CreateNewPlayerSlotHBox();

	// horizontal slot을 조정하기
    void SetHorizontalSlot(class UHorizontalBoxSlot *hSlot, enum EHorizontalAlignment align, float sizeValue = 1.0f);

public:
	// 플레이어 id 배열 받아서 슬롯 ui 추가
	UFUNCTION(BlueprintCallable)
    void AddPlayerSlot(TArray<FString> playerIdAry);
};
