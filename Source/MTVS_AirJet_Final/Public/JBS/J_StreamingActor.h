// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KHS/K_StreamingActor.h"
#include "J_StreamingActor.generated.h"

/**
 * 
 */
UCLASS()
class MTVS_AIRJET_FINAL_API AJ_StreamingActor : public AK_StreamingActor
{
	GENERATED_BODY()
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Values")
	bool isInitScreen = false;

public:

protected:
    virtual void BeginPlay() override;

	// 스크린 초기화 시도
    bool TryInitScreen();

public:
    virtual void Tick(float DeltaTime) override;

	// 주어진 사용자 ID를 설정하여 화면 공유를 시작하는 함수 | 캐릭터 클래스 변경
    virtual void SetViewSharingUserID(FString ID, const bool &bAddPlayer) override;
};
