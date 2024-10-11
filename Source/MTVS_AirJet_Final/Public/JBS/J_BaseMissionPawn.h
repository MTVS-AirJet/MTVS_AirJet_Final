// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "J_BaseMissionPawn.generated.h"

UCLASS()
class MTVS_AIRJET_FINAL_API AJ_BaseMissionPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AJ_BaseMissionPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
protected:


public:

protected:


public:
	// 픽셀 스트리밍용 함수
	UFUNCTION(Server, Reliable)
	void ServerRPC_SetStreamingPlayer(const FString &playerId, bool bAddPlayer);

        // ui와는 분리함
	// class UK_StreamingUI* streamingUI;
	// void InitStreamingUI();
};
