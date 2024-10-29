// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "L_RoadTrigger.generated.h"

UCLASS()
class MTVS_AIRJET_FINAL_API AL_RoadTrigger : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AL_RoadTrigger();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditdefaultsOnly , Category="Components")
	class UBoxComponent* RoadTriggerComp;

public:
	UPROPERTY(EditAnywhere , Category="Trigger")
	int32 TriggerIdx = 0;
};
