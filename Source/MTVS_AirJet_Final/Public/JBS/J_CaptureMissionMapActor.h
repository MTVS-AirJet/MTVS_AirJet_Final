// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "J_CaptureMissionMapActor.generated.h"

UCLASS()
class MTVS_AIRJET_FINAL_API AJ_CaptureMissionMapActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AJ_CaptureMissionMapActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Component")
	class USceneComponent* sceneRoot;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Component")
	class USceneCaptureComponent2D* missionMapCapture2D;


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Values", ReplicatedUsing=OnRep_Fov)
	float fov = 90.f;
		public:
	__declspec(property(get = GetFov, put = SetFov)) float FOV;
	float GetFov()
	{
		return fov;
	}
	UFUNCTION(BlueprintSetter)
	void SetFov(float value);

    protected:
    public:
    protected:

	UFUNCTION()
	void OnRep_Fov();

public:

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;
};
