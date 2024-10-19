// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "L_Missile.generated.h"

UCLASS()
class MTVS_AIRJET_FINAL_API AL_Missile : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AL_Missile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(EditDefaultsOnly , Category="Components" , BlueprintReadWrite)
	class UStaticMeshComponent* MissileMesh;
	UPROPERTY(EditDefaultsOnly , Category="Components")
	class UBoxComponent* MissileBoxComp;
};
