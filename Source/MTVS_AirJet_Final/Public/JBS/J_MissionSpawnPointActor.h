// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <JBS/J_Utility.h>
#include "J_MissionSpawnPointActor.generated.h"

UCLASS()
class MTVS_AIRJET_FINAL_API AJ_MissionSpawnPointActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AJ_MissionSpawnPointActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// 위치 및 방향 표시
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Component")
	class UArrowComponent* rootComp;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
	EPlayerRole spawnType;

protected:


public:
};
