// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
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
	UPROPERTY(EditDefaultsOnly , Category="Default|Components")
	class UStaticMeshComponent* FlareMesh;
	UPROPERTY(EditDefaultsOnly , Category="Default|Components")
	class UBoxComponent* FlareBoxComp;
	UPROPERTY(EditDefaultsOnly , Category="Default|Move")
	float FlareSpeed = 1000.f;

public:
	UPROPERTY(EditDefaultsOnly , Category="Default|VFX" , BlueprintReadWrite)
	class UNiagaraComponent* FlareEffect;
	UPROPERTY(EditDefaultsOnly , Category="Default|VFX" , BlueprintReadWrite)
	class USplineComponent* FlareSpline;

private:
	UFUNCTION()
	void OnFlareBeginOverlap(UPrimitiveComponent* OverlappedComponent , AActor* OtherActor ,
	                         UPrimitiveComponent* OtherComp , int32 OtherBodyIndex , bool bFromSweep ,
	                         const FHitResult& SweepResult);

private:
	UPROPERTY(EditDefaultsOnly , Category="Default|Move")
	class UCurveFloat* FlareCurve;
	UPROPERTY()
	FTimeline FlareTimeline;
	UFUNCTION()
	void FlareUpdate(float Alpha);
	UFUNCTION()
	void FlareFinished();

	FVector BezierFlare(FVector P1 , FVector P2 , FVector P3 , FVector P4 , float Alpha);

	TArray<FVector> MoveLoc;
	
	bool bPursuit;

private:
	UPROPERTY(EditDefaultsOnly , Category="Default|VFX")
	float MaxLifeTime = 3.f;
	UPROPERTY(EditDefaultsOnly , Category="Default|VFX")
	float MinLifeTime = 50.f;
	UPROPERTY(EditDefaultsOnly , Category="Default|VFX")
	float MaxScaleSprite = 1200.f;
	UPROPERTY(EditDefaultsOnly , Category="Default|VFX")
	float MinScaleSprite = 300.f;
	UPROPERTY(EditDefaultsOnly , Category="Default|VFX")
	float SpawnRadius = 0.f;
	UPROPERTY(EditDefaultsOnly , Category="Default|VFX")
	float SpawnRate = 50.f;
	UPROPERTY(EditDefaultsOnly , Category="Default|VFX")
	FLinearColor LColor = FLinearColor(0.147027 , 0.147027 , 0.147027 , 1);

private:
	UPROPERTY(EditDefaultsOnly , Category="Default|Sound")
	class UAudioComponent* AudioComponent;
};
