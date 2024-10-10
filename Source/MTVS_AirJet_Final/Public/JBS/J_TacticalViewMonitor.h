// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "J_TacticalViewMonitor.generated.h"

UCLASS()
class MTVS_AIRJET_FINAL_API AJ_TacticalViewMonitor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AJ_TacticalViewMonitor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Component")
	class USceneComponent* rootComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Component")
	class USceneComponent* monitorRootComp;
	// 모니터 UI comp
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Component")
	class UWidgetComponent* monitorUIComp;
	// 전술화면 ui
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|UI")
	class UJ_TacticalViewUI* tacViewUI;

	//미션 맵 캡쳐 카메라
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Objects")
	class AJ_CaptureMissionMapActor* mapCaptureActor;

	// 맵 fov 최대/최소
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values|MapCam")
	float minFov = 30.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values|MapCam")
	float maxFov = 90.f;
	// 현재 fov 확대 퍼센티지 : fov 값 alpha 로 사용
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values|MapCam")
	float fovPercent = 1.f;

public:
	// 지도 확대/축소
	UFUNCTION(BlueprintCallable)
	void SetMapFov(float value);

    protected:
    public:
};
