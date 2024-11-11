// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "L_Target.generated.h"

UCLASS()
class MTVS_AIRJET_FINAL_API AL_Target : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AL_Target();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditDefaultsOnly , category="Default|Components")
	class UWidgetComponent* TargetWidget;
	
private:
	UPROPERTY()
	class AL_Viper* MyViper;

public:
	void F_Destroy();

private:
	UFUNCTION()
	void PrintNetLog();
};
