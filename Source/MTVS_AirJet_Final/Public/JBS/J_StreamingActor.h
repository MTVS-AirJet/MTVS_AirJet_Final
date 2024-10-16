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

	// ps서버 url
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
	FString psServerURL = TEXT("ws://125.132.216.190:7755");


public:

protected:
    virtual void BeginPlay() override;

	// 스크린 초기화 시도
    bool TryInitScreen();

public:
    virtual void Tick(float DeltaTime) override;

	// 주어진 사용자 ID를 설정하여 화면 공유를 시작하는 함수 | 캐릭터 클래스 변경
    virtual void SetViewSharingUserID(FString ID, const bool &bAddPlayer) override;

	// 화면 공유를 다른 사용자 시점으로 변경하는 함수 : bp 함수 쓰던걸 변경
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void ChangeLookSharingScreen() override;

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void BeginLookSharingScreen() override;

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void StopLookSharingScreen() override;

	// FIXME 연결 되니깐 이제 화면공유를 태블릿쪽으로 변경하고 플레이어와 UI 분리 하기
};
