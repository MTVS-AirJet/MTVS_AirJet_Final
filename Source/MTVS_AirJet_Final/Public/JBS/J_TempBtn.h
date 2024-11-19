// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "JBS/J_CustomButton.h"
#include "J_TempBtn.generated.h"

/**
 * 
 */
UCLASS()
class MTVS_AIRJET_FINAL_API UJ_TempBtn : public UUserWidget
{
	GENERATED_BODY()
protected:
	/* ========= WBP UI 클래스 ===========*/
	// 커스텀 버튼
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|UI", meta = (BindWidget))
	class UJ_CustomButton* J_CustomButton_1;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|UI", meta = (BindWidget))
	class UJ_CustomButton* J_CustomButton_2;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|UI", meta = (BindWidget))
	class UJ_CustomButton* J_CustomButton_3;
public:
protected:
    virtual void NativeConstruct() override{
		Super::NativeConstruct();
		// oneparam 딜리게이트에 사용할 함수 바인드
		J_CustomButton_1->onClickOneParam.AddDynamic( this, &UJ_TempBtn::UseFunction);
		J_CustomButton_1->customValue = 1; // 여기서 하지않고 블루프린트에서 설정 할 수도 있음
		J_CustomButton_2->onClickOneParam.AddDynamic( this, &UJ_TempBtn::UseFunction);
		// J_CustomButton_1->customValue = 2;
		J_CustomButton_3->onClickOneParam.AddDynamic( this, &UJ_TempBtn::UseFunction);
		// J_CustomButton_1->customValue = 3;
	}

	// 함수 사용 | 하나로 축약되어서 편리
	UFUNCTION(BlueprintCallable)
	void UseFunction(int idx)
	{
		GEngine->AddOnScreenDebugMessage(-1, 333.f, FColor::Green, FString::Printf(TEXT("idx : %d"), idx));
	}

public:
};
