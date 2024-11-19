// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "J_CustomButton.generated.h"

// int 파라미터를 받을 수 있는 딜리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnClickOPDel, int, idx);

UCLASS()
class MTVS_AIRJET_FINAL_API UJ_CustomButton : public UButton
{
	GENERATED_BODY()
protected:
public:
	// 딜리게이트 변수
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, BlueprintAssignable, Category="Default|Delegate")
	FOnClickOPDel onClickOneParam;
	// 인자로 보낼 값
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
	int customValue = 0;
protected:
	// 버튼 초기화 하는 함수 오버라이드
	virtual TSharedRef<SWidget> RebuildWidget() override {
		auto result = Super::RebuildWidget();
		// OnClick에 내 함수 바인드
		OnClicked.AddDynamic( this, &UJ_CustomButton::OnClickCustomButton);
		return result;
	}

	// OnClick 될때 실행되는 내 함수
	UFUNCTION(BlueprintCallable)
	void OnClickCustomButton() {
		// int 인자로 보내기
		onClickOneParam.Broadcast(customValue);
	}

public:	
};
