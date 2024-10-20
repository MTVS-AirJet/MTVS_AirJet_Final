﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "JBS/J_BaseMissionPawn.h"
#include "L_Viper.generated.h"

UCLASS()
class MTVS_AIRJET_FINAL_API AL_Viper : public AJ_BaseMissionPawn
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
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

private: // Component
	UPROPERTY(EditDefaultsOnly , Category="Components")
	class UBoxComponent* JetRoot;
	UPROPERTY(EditDefaultsOnly , Category="Components")
	class USkeletalMeshComponent* JetMesh;
	UPROPERTY(EditDefaultsOnly , Category="Components")
	class UArrowComponent* JetArrow;
	UPROPERTY(EditdefaultsOnly , Category="Components")
	class UBoxComponent* JetFirstEngine;
	UPROPERTY(EditdefaultsOnly , Category="Components")
	class UBoxComponent* JetMic;
	UPROPERTY(EditdefaultsOnly , Category="Components")
	class UBoxComponent* JetEngineMaster;
	UPROPERTY(EditdefaultsOnly , Category="Components")
	class UBoxComponent* JetEngineControl;
	UPROPERTY(EditdefaultsOnly , Category="Components")
	class UBoxComponent* JetFuelStarter;
	UPROPERTY(EditDefaultsOnly , category="Components")
	class UWidgetComponent* JetWidget;

	UPROPERTY(EditDefaultsOnly , category="Components")
	class UCharacterMovementComponent* movement;
	UPROPERTY(EditDefaultsOnly , category="Components")
	class UNiagaraComponent* BoosterLeftVFX;
	UPROPERTY(EditDefaultsOnly , category="Components")
	class UNiagaraComponent* BoosterRightVFX;

public:
	UPROPERTY(EditDefaultsOnly , Category="Components" , BlueprintReadWrite)
	class USpringArmComponent* JetSprintArm;
	UPROPERTY(EditDefaultsOnly , Category="Components" , BlueprintReadWrite)
	class UCameraComponent* JetCamera;
	UPROPERTY(EditDefaultsOnly , Category="Components" , BlueprintReadWrite)
	class USpringArmComponent* JetSprintArmFPS;
	UPROPERTY(EditDefaultsOnly , Category="Components" , BlueprintReadWrite)
	class UCameraComponent* JetCameraFPS;

private:
	UFUNCTION()
	void OnMyFirstEngineClicked(UPrimitiveComponent* TouchedComponent , struct FKey ButtonPressed);
	UFUNCTION()
	void OnMyMicClicked(UPrimitiveComponent* TouchedComponent , struct FKey ButtonPressed);
	UFUNCTION()
	void OnMyEngineMasterClicked(UPrimitiveComponent* TouchedComponent , struct FKey ButtonPressed);
	UFUNCTION()
	void OnMyEngineControlClicked(UPrimitiveComponent* TouchedComponent , struct FKey ButtonPressed);
	UFUNCTION()
	void OnMyJetFuelStarterClicked(UPrimitiveComponent* TouchedComponent , struct FKey ButtonPressed);

private: // Input
	UPROPERTY(EditDefaultsOnly , Category="Inputs")
	class UInputMappingContext* IMC_Viper;
	UPROPERTY(EditDefaultsOnly , Category="Inputs")
	class UInputAction* IA_ViperEngine;
	UPROPERTY(EditDefaultsOnly , Category="Inputs")
	class UInputAction* IA_ViperLook;
	UPROPERTY(EditDefaultsOnly , Category="Inputs")
	class UInputAction* IA_ViperUp;
	UPROPERTY(EditDefaultsOnly , Category="Inputs")
	class UInputAction* IA_ViperDown;
	UPROPERTY(EditDefaultsOnly , Category="Inputs")
	class UInputAction* IA_ViperLeft;
	UPROPERTY(EditDefaultsOnly , Category="Inputs")
	class UInputAction* IA_ViperRight;
	UPROPERTY(EditDefaultsOnly , Category="Inputs")
	class UInputAction* IA_ViperTurnLeft;
	UPROPERTY(EditDefaultsOnly , Category="Inputs")
	class UInputAction* IA_ViperTurnRight;
	UPROPERTY(EditDefaultsOnly , Category="Inputs")
	class UInputAction* IA_ViperResetRotation;
	UPROPERTY(EditDefaultsOnly , Category="Inputs")
	class UInputAction* IA_ViperAccel;
	UPROPERTY(EditDefaultsOnly , Category="Inputs")
	class UInputAction* IA_ViperBreak;
	UPROPERTY(EditDefaultsOnly , Category="Inputs")
	class UInputAction* IA_ViperShoot;
	UPROPERTY(EditDefaultsOnly , Category="Inputs")
	class UInputAction* IA_ViperFPS;
	UPROPERTY(EditDefaultsOnly , Category="Inputs")
	class UInputAction* IA_ViperTPS;

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
	UFUNCTION()
	void F_ViperShootStarted(const struct FInputActionValue& value);
	UFUNCTION()
	void F_ViperFpsStarted(const struct FInputActionValue& value);
	UFUNCTION()
	void F_ViperTpsStarted(const struct FInputActionValue& value);

private:
	bool bFirstEngine;
	// For Engine Using 
	bool IsEngineOn;
	// For Change Arrow Rotate
	float CurrentTime;
	UPROPERTY(EditDefaultsOnly , Category="Inputs")
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
	UPROPERTY(EditDefaultsOnly , Category="Rotate")
	float MaxPitchValue = 85.f;
	UPROPERTY(EditDefaultsOnly , Category="Rotate")
	float MinPitchValue = -85.f;
	UPROPERTY(EditDefaultsOnly , Category="Rotate")
	float MaxYawValue = 80.f;
	UPROPERTY(EditDefaultsOnly , Category="Rotate")
	float MinYawValue = -80.f;

private:
	UPROPERTY(EditDefaultsOnly , Category="Movements")
	float MaxValueOfMoveForce = 305580000.f;
	UPROPERTY(EditDefaultsOnly , Category="Movements")
	float ValueOfMoveForce = 0.0f;
	UPROPERTY(EditDefaultsOnly , Category="Movements")
	float ValueOfHeightForce = 1.f;
	UPROPERTY(EditDefaultsOnly , Category="Movements")
	FVector HeightForceLoc = FVector(1000 , 0 , 0);
	UPROPERTY(EditDefaultsOnly , Category="Movements")
	float ValueOfDivRot = 50.f;
	// UPROPERTY(EditDefaultsOnly, Category="Movements")
	// float ValueOfArrowReset = -.1f;

private:
	UPROPERTY(EditDefaultsOnly , Category="Movements")
	float ChangeAccelTime = 1.f; // 다음 기어가 되기까지 소요되는 시간
	float KeyDownAccel = 0.f; // 액셀 누르고 있는 시간

	bool IsAccel = false;
	bool IsBreak = false;
	int32 AccelGear = 0; // 현재 엑셀 기어 (0=0%,1=50%,2=100%,3=100% + 애프터버너)
	float GetAddTickSpeed();
	// 기어 0
	UPROPERTY(EditDefaultsOnly , category = "ZeroGear")
	float ZeroGearFlight = -20.f;
	// 수평비행
	UPROPERTY(EditDefaultsOnly , category = "BasicFlight")
	float BasicFlight50 = 20.f;
	UPROPERTY(EditDefaultsOnly , category = "BasicFlight")
	float BasicFlight100 = 50.f;
	UPROPERTY(EditDefaultsOnly , category = "BasicFlight")
	float BasicFlightAB = 100.f;
	UPROPERTY(EditDefaultsOnly , category = "UpFlight")
	float UpFlight50 = -20.f;
	UPROPERTY(EditDefaultsOnly , category = "UpFlight")
	float UpFlight100 = 0.f;
	UPROPERTY(EditDefaultsOnly , category = "UpFlight")
	float UpFlightAB = 10.f;
	UPROPERTY(EditDefaultsOnly , category = "DownFlight")
	float DownFlight50 = 70.f;
	UPROPERTY(EditDefaultsOnly , category = "DownFlight")
	float DownFlight100 = 110.f;
	UPROPERTY(EditDefaultsOnly , category = "DownFlight")
	float DownFlightAB = 150.f;
	UPROPERTY(EditDefaultsOnly , category = "TurnFlight")
	float TurnFlight50 = -30.f;
	UPROPERTY(EditDefaultsOnly , category = "TurnFlight")
	float TurnFlight100 = -5.f;
	UPROPERTY(EditDefaultsOnly , category = "TurnFlight")
	float TurnFlightAB = 20.f;

private:
	UPROPERTY(EditDefaultsOnly , Category="HUD")
	float HeightOfSea = -480000; // Cesium 해수면 높이
	UPROPERTY(EditDefaultsOnly , Category="HUD")
	TSubclassOf<class UUserWidget> HUD_UI;

private:
	UFUNCTION()
	void IsLockOn();

	float Diametr = 30.f;
	UPROPERTY(EditDefaultsOnly , Category="Attack")
	TSubclassOf<class AL_Missile> Missile;

public:
	UPROPERTY(EditDefaultsOnly , Category="Attack")
	int32 RangeCnt = 6;

public:
	UPROPERTY(EditDefaultsOnly , Category="Attack" , BlueprintReadWrite)
	AActor* LockOnTarget = nullptr;
	UPROPERTY(EditDefaultsOnly , Category="Attack" , BlueprintReadWrite)
	class USceneComponent* MissileMoveLoc;

private:
	
	UFUNCTION()
	void ChangeBooster();

private:
	UFUNCTION()
	void PrintNetLog();

private:
	UFUNCTION(Server, Reliable)
	void ServerRPCLocationAndRotation(FVector newLocaction, FRotator newRotator);
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCLocationAndRotation(FVector newLocaction, FRotator newRotator);

	UFUNCTION(Server, Reliable)
	void ServerRPCBoost(bool isOn);
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCBoost(bool isOn);

	UFUNCTION(Server, Reliable)
	void ServerRPCLockOn();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCLockOn(AActor* target);
	
	UFUNCTION(Server, Reliable)
	void ServerRPCMissile(AActor* newOwner);
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCMissile(AActor* newOwner);
};
