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
	class UBoxComponent* JetRoot;
	UPROPERTY(EditDefaultsOnly)
	class USkeletalMeshComponent* JetMesh;
	UPROPERTY(EditDefaultsOnly)
	class USpringArmComponent* JetSprintArm;
	UPROPERTY(EditDefaultsOnly)
	class UCameraComponent* JetCamera;
	UPROPERTY(EditDefaultsOnly)
	class UArrowComponent* JetArrow;

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
	UPROPERTY(EditDefaultsOnly)
	class UInputAction* IA_ViperTurnLeft;
	UPROPERTY(EditDefaultsOnly)
	class UInputAction* IA_ViperTurnRight;
	UPROPERTY(EditDefaultsOnly)
	class UInputAction* IA_ViperResetRotation;
	UPROPERTY(EditDefaultsOnly)
	class UInputAction* IA_ViperAccel;
	UPROPERTY(EditDefaultsOnly)
	class UInputAction* IA_ViperBreak;

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
	UFUNCTION()
	void F_ViperTurnRightTrigger(const struct FInputActionValue& value);
	UFUNCTION()
	void F_ViperTurnRightCompleted(const struct FInputActionValue& value);
	UFUNCTION()
	void F_ViperTurnLeftTrigger(const struct FInputActionValue& value);
	UFUNCTION()
	void F_ViperTurnLeftCompleted(const struct FInputActionValue& value);
	UFUNCTION()
	void F_ViperResetRotation(const struct FInputActionValue& value);
	UFUNCTION()
	void F_ViperAccelStarted(const struct FInputActionValue& value);
	UFUNCTION()
	void F_ViperAccelCompleted(const struct FInputActionValue& value);
	UFUNCTION()
	void F_ViperBreakStarted(const struct FInputActionValue& value);
	UFUNCTION()
	void F_ViperBreakCompleted(const struct FInputActionValue& value);

private:
	// For Engine Using 
	bool IsEngineOn;
	// For Change Arrow Rotate
	float CurrentTime;
	UPROPERTY(EditDefaultsOnly)
	float ChangeTime = .3f;
	// For Reset Arrow Rotate
	bool IsKeyUpPress;
	bool IsKeyDownPress;
	bool IsKeyRightPress;
	bool IsKeyLeftPress;
	// Rotate vector
	bool IsRightRoll;
	bool IsLeftRoll;
	UPROPERTY(EditDefaultsOnly)
	FVector ChangeMoveVector = FVector(0 , .5f , 0);
	UPROPERTY(EditDefaultsOnly)
	FRotator RotateValue = FRotator(0 , 0 , .5f);
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
	float ValueOfMoveForce = 185200.0f;
	UPROPERTY(EditDefaultsOnly)
	float ValueOfHeightForce = 1.f;
	UPROPERTY(EditDefaultsOnly)
	FVector HeightForceLoc = FVector(1000 , 0 , 0);
	UPROPERTY(EditDefaultsOnly)
	float ValueOfDivRot = 50.f;
	UPROPERTY(EditDefaultsOnly)
	float ValueOfArrowReset = -.1f;

private:
	UPROPERTY(EditDefaultsOnly)
	float MaxAccelValue = 150.f;
	float CurrAccelValue = 0.f;
	bool IsAccel = false;
	bool IsBreak = false;
};
