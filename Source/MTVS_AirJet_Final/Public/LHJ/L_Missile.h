// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
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

private:
	UPROPERTY(EditDefaultsOnly , Category="Defalut|Components" )
	class UStaticMeshComponent* MissileMesh;
	UPROPERTY(EditDefaultsOnly , Category="Defalut|Components")
	class UBoxComponent* MissileBoxComp;
	UPROPERTY(EditDefaultsOnly , Category="Defalut|Components")
	class UNiagaraComponent* EngineVFX;
	UPROPERTY(EditDefaultsOnly , Category="Defalut|Components")
	class UNiagaraComponent* SmokeVFX;

	UPROPERTY(EditDefaultsOnly , Category="Default|Sound")
	class UAudioComponent* AudioComponent;
		
private:
	UPROPERTY(EditDefaultsOnly , Category="Default|Move")
	class UCurveFloat* MissileCurve;
	UPROPERTY()
	FTimeline MissileTimeline;
	UFUNCTION()
	void MissileUpdate(float Alpha);
	UFUNCTION()
	void MissileFinished();

	FVector BezierMissile(FVector P1 , FVector P2 , FVector P3 , FVector P4 , float Alpha);

private:
	UPROPERTY(EditDefaultsOnly , Category="Default|Move")
	AActor* Target = nullptr;

	TArray<FVector> MoveLoc;

	bool bPursuit;

	UPROPERTY(EditDefaultsOnly , Category="Default|Move")
	float MoveSpeed = 500.f;

private:
	UFUNCTION()
	void OnMissileBeginOverlap(UPrimitiveComponent* OverlappedComponent , AActor* OtherActor ,
	                           UPrimitiveComponent* OtherComp , int32 OtherBodyIndex , bool bFromSweep ,
	                           const FHitResult& SweepResult);

	UFUNCTION(Server, Reliable)
	void ServerRPCDamage(AActor* HitActor);
};
