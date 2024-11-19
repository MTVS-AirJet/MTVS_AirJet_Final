// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "J_DetailUI.generated.h"

UCLASS()
class MTVS_AIRJET_FINAL_API UJ_DetailUI : public UUserWidget
{
	GENERATED_BODY()
protected:
	// 이미지 맵( IDX, 텍스쳐 )
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Classes")
	// TMap<int, 

public:

protected:


public:
    void SetDetailUI(int idx);
};
