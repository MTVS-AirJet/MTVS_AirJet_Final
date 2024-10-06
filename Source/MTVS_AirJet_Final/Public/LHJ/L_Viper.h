// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "L_Viper.generated.h"

UCLASS()
class MTVS_AIRJET_FINAL_API AL_Viper : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AL_Viper();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private: // Component
	UPROPERTY(EditDefaultsOnly)
	class USkeletalMeshComponent* JetMesh;
	UPROPERTY(EditDefaultsOnly)
	class USpringArmComponent* JetSprintArm;
	UPROPERTY(EditDefaultsOnly)
	class UCameraComponent* JetCamera;
	UPROPERTY(EditDefaultsOnly)
	class UArrowComponent* JetArrow;
public:
	UPROPERTY(EditDefaultsOnly)
	class UStaticMeshComponent* JetStaticMesh;

private: // Input
	UPROPERTY(EditDefaultsOnly)
	class UInputMappingContext* IMC_Viper;
	UPROPERTY(EditDefaultsOnly)
	class UInputAction* IA_ViperEngine;
	UPROPERTY(EditDefaultsOnly)
	class UInputAction* IA_ViperLook;
	UPROPERTY(EditDefaultsOnly)
	class UInputAction* IA_ViperUp;
	UPROPERTY(EditDefaultsOnly)
	class UInputAction* IA_ViperDown;
	UPROPERTY(EditDefaultsOnly)
	class UInputAction* IA_ViperLeft;
	UPROPERTY(EditDefaultsOnly)
	class UInputAction* IA_ViperRight;

	UFUNCTION()
	void F_ViperEngine(const struct FInputActionValue& value);
	UFUNCTION()
	void F_ViperLook(const struct FInputActionValue& value);
	UFUNCTION()
	void F_ViperUpTrigger(const struct FInputActionValue& value);
	UFUNCTION()
	void F_ViperUpCompleted(const struct FInputActionValue& value);
	UFUNCTION()
	void F_ViperDownTrigger(const struct FInputActionValue& value);
	UFUNCTION()
	void F_ViperDownCompleted(const struct FInputActionValue& value);
	UFUNCTION()
	void F_ViperRightTrigger(const struct FInputActionValue& value);
	UFUNCTION()
	void F_ViperRightCompleted(const struct FInputActionValue& value);
	UFUNCTION()
	void F_ViperLeftTrigger(const struct FInputActionValue& value);
	UFUNCTION()
	void F_ViperLeftCompleted(const struct FInputActionValue& value);

private:
	// For Engine Using 
	bool IsEngineOn;
	// For Change Arrow Rotate
	float CurrentTime;
	float ChangeTime = .3f;
	// For Reset Arrow Rotate
	bool IsKeyUpPress;
	bool IsKeyDownPress;
	bool IsKeyRightPress;
	bool IsKeyLeftPress;
	// Rotate Value
	FRotator ForceUnitRot;
	// Add Rotate Value ( ForceUnitRot To NewVector )
	FRotator CombineRotate(FVector NewVector);

private:
	// For Check Distance
	UPROPERTY(EditDefaultsOnly)
	float MaxPitchValue = 85.f;
	UPROPERTY(EditDefaultsOnly)
	float MinPitchValue = -85.f;
	UPROPERTY(EditDefaultsOnly)
	float MaxYawValue = 80.f;
	UPROPERTY(EditDefaultsOnly)
	float MinYawValue = -80.f;

private:
	UPROPERTY(EditDefaultsOnly)
	float ValueOfMoveForce = 5000000.0f;
	UPROPERTY(EditDefaultsOnly)
	float ValueOfHeightForce = 1.f;
	UPROPERTY(EditDefaultsOnly)
	FVector HeightForceLoc = FVector(1000 , 0 , 0);
	UPROPERTY(EditDefaultsOnly)
	float ValueOfDivRot = 50.f;
	UPROPERTY(EditDefaultsOnly)
	float ValueOfArrowReset = -.1f;
};
