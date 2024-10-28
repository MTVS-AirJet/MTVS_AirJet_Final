// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "L_Flare.generated.h"

UCLASS()
class MTVS_AIRJET_FINAL_API AL_Flare : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AL_Flare();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditDefaultsOnly , Category="Components")
	class UStaticMeshComponent* FlareMesh;
	UPROPERTY(EditDefaultsOnly , Category="Components")
	class UBoxComponent* FlareBoxComp;
	UPROPERTY(EditDefaultsOnly , Category="Speed")
	float FlareSpeed = 1000.f;

public:
	UPROPERTY(EditDefaultsOnly , Category="Components" , BlueprintReadWrite)
	class UNiagaraComponent* FlareEffect;
	UPROPERTY(EditDefaultsOnly , Category="Components" , BlueprintReadWrite)
	class USplineComponent* FlareSpline;

private:
	UFUNCTION()
	void OnFlareBeginOverlap(UPrimitiveComponent* OverlappedComponent , AActor* OtherActor ,
	                         UPrimitiveComponent* OtherComp , int32 OtherBodyIndex , bool bFromSweep ,
	                         const FHitResult& SweepResult);

	UFUNCTION(Server , Reliable)
	void ServerRPCFlare();

private:
	UPROPERTY(EditDefaultsOnly , Category="Effect")
	float MaxLifeTime = 3.f;
	UPROPERTY(EditDefaultsOnly , Category="Effect")
	float MinLifeTime = 50.f;
	UPROPERTY(EditDefaultsOnly , Category="Effect")
	float MaxScaleSprite = 1200.f;
	UPROPERTY(EditDefaultsOnly , Category="Effect")
	float MinScaleSprite = 300.f;
	UPROPERTY(EditDefaultsOnly , Category="Effect")
	float SpawnRadius = 0.f;
	UPROPERTY(EditDefaultsOnly , Category="Effect")
	float SpawnRate = 50.f;
	UPROPERTY(EditDefaultsOnly , Category="Effect")
	FLinearColor LColor = FLinearColor(0.147027 , 0.147027 , 0.147027 , 1);
};
