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
public:
	//발사체 이동을 담당할 컴포넌트
	UPROPERTY(EditDefaultsOnly, Category = Movement)
	class UProjectileMovementComponent* movementComp;
	
	class UProjectileMovementComponent* GetProjectileMovementComponent() const;

private:
	UFUNCTION()
	void OnFlareBeginOverlap(UPrimitiveComponent* OverlappedComponent , AActor* OtherActor ,
	                           UPrimitiveComponent* OtherComp , int32 OtherBodyIndex , bool bFromSweep ,
	                           const FHitResult& SweepResult);

	UFUNCTION(Server, Reliable)
	void ServerRPCFlare();
};
