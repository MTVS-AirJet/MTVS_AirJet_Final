// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "J_CircleProgressBar.generated.h"

/**
 * 
 */
UCLASS()
class MTVS_AIRJET_FINAL_API UJ_CircleProgressBar : public UUserWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values", BlueprintGetter=GetPercent, BlueprintSetter=SetPercent)
	float percent = 0.f;
		public:
	__declspec(property(get = GetPercent, put = SetPercent)) float PERCENT;
	UFUNCTION(BlueprintCallable)
	float GetPercent() const
	{
		return percent;
	}
	UFUNCTION(BlueprintCallable)
	void SetPercent(float value)
	{
		percent = value;
		SetPercentMat(percent);
	}
		protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values", BlueprintGetter=GetThickness, BlueprintSetter=SetThickness)
	float thickness = 0.f;
		public:
	__declspec(property(get = GetThickness, put = SetThickness)) float THICKNESS;
	UFUNCTION(BlueprintCallable)
	float GetThickness() const
	{
		return thickness;
	}
	UFUNCTION(BlueprintCallable)
	void SetThickness(float value)
	{
		thickness = value;
		SetThicknessMat(thickness);
	}
		protected:

public:

protected:
	// 머티리얼에 파라미터 적용
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void SetPercentMat(float amount);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void SetThicknessMat(float amount);
public:
	
};
