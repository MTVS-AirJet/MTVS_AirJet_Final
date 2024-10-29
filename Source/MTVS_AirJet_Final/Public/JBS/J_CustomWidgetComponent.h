// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "J_CustomWidgetComponent.generated.h"

/**
 * 
 */
UCLASS()
class MTVS_AIRJET_FINAL_API UJ_CustomWidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()
protected:
	// 기준 거리 값
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
	float baseDistance = 2500.f;
	// 원래 스케일
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Values")
	FVector orgUIScale;
	// 로컬 pc
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Objects")
	APlayerController* localPC;
	// 목표 액터 | 폰
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Objects")
	AActor* targetActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
	bool enableBillboard = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
	bool enableFixScale = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
	bool enableFixHeight = true;
public:
	

protected:
    virtual void BeginPlay() override;

	// 빌보드 처리
    virtual void Billboard(APlayerController *pc);
    // 크기 고정 처리
    virtual void FixScale(AActor *target);

    virtual void FixScale(FVector targetLocation);

	// 눈높이 맞춤 처리
    virtual void FixHeight(APlayerController *pc);

public:
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void SetTargetActor(AActor* actor)
	{
		targetActor = actor;
	}
};
