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


	// 이미지 맵( 미션 절차, 텍스쳐 ) | 컨트롤러
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Classes")
	TMap<EMissionProcess, FSlateBrush> imgMapDefault;

	// 이미지 맵 | 키마
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Classes")
	TMap<EMissionProcess, FSlateBrush> imgMapKeyboard;

	
	
	// 현재 인풋
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
	EInputDevice curInput;
		public:
	__declspec(property(get = GetCurInput, put = SetCurInput)) EInputDevice CUR_INPUT;
	EInputDevice GetCurInput()
	{
		return curInput;
	}
	UFUNCTION(BlueprintSetter, BlueprintCallable)
	void SetCurInput(EInputDevice value)
	{
		curInput = value;
		SetDetailImg(curImgKey);
	}
		protected:


public:
	// 현재 이미지 키
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Values")
	EMissionProcess curImgKey;


protected:
	// idx 값 보정
    void AdjustIdx(int &idx);

public:
    void SetDetailUI(int idx);
    void SetDetailUI(const EMissionProcess& value);
	// 이미지 변경
	UFUNCTION(BlueprintCallable)
	void SetDetailImg(const EMissionProcess &value);
        // 이미지 갱신 애니메이션
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void PlayUpdateAnim();

	// 이미지 가져오기
	UFUNCTION(BlueprintCallable)
	FSlateBrush GetImg(const EMissionProcess& process);
};
