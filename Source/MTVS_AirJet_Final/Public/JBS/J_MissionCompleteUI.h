// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "J_MissionCompleteUI.generated.h"

/**
 * 
 */
// @@ pc에 로비로 돌아가기 바인드할 딜리게이트 추가

UCLASS()
class MTVS_AIRJET_FINAL_API UJ_MissionCompleteUI : public UUserWidget
{
	GENERATED_BODY()
protected:
	// 수행 결과 리스트
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|UI", meta = (BindWidget))
	class UVerticalBox* objListVBox;

	// 로비 돌아가기 버튼
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|UI", meta = (BindWidget))
	class UButton* MC_BackToLobbyBtn;


	// 수행 결과 요소 프리팹
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Classes")
	TSubclassOf<class UJ_MissionCompleteObjElement> objElementPrefab;
public:

protected:
	// 요소 추가

public:
	// 결과 값 받기
};
