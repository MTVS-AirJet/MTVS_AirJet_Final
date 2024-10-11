// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <JBS/J_BaseMissionPawn.h>
#include "J_CommanderPlayer.generated.h"

UCLASS()
class MTVS_AIRJET_FINAL_API AJ_CommanderPlayer : public AJ_BaseMissionPawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AJ_CommanderPlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	// 지휘관 imc
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Classes")
	class UInputMappingContext* imcCommander;

	// 전술 모니터
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Objects")
	class AJ_TacticalViewMonitor* tacViewMonitor;

	// 지도 스크롤 감도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
	float mapScrollSensitivity = 5.f;


public:

protected:
    virtual void PossessedBy(AController *newController) override;

	// 맵 스크롤(확대/축소) 입력
	UFUNCTION(BlueprintCallable)
	void EventScroll(float value);

	UFUNCTION(Server, Reliable)
	void SRPC_MapScroll(float value);

    public:

};
