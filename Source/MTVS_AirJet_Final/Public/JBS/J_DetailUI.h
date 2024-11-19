// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "J_Utility.h"
#include "J_DetailUI.generated.h"

UCLASS()
class MTVS_AIRJET_FINAL_API UJ_DetailUI : public UUserWidget
{
	GENERATED_BODY()
protected:
	// 상세 이미지
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|UI", meta = (BindWidget))
	class UImage* detailImage;


	// 이미지 맵( 미션 절차, 텍스쳐 )
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Classes")
	TMap<EMissionProcess, FSlateBrush> imgMap;

	// 현재 이미지 키
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Values")
	EMissionProcess curImgKey;
	



public:

protected:
	// idx 값 보정
    void AdjustIdx(int &idx);

public:
    void SetDetailUI(int idx);
    void SetDetailUI(const EMissionProcess& value);
};
