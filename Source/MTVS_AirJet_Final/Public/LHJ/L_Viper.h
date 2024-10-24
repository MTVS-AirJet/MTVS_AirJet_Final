﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "JBS/J_BaseMissionPawn.h"
#include "L_Viper.generated.h"

UENUM()
enum class EWeapon
{
	Missile = 0 ,
	Flare ,
	// 항상 마지막에 추가하여 열거형의 크기를 구할 수 있게 함
	Max
};


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

private:
	UFUNCTION()
	void PrintNetLog();

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
	class UBoxComponent* JetEngineMaster1;
	UPROPERTY(EditdefaultsOnly , Category="Components")
	class UBoxComponent* JetEngineGen;
	UPROPERTY(EditdefaultsOnly , Category="Components")
	class UBoxComponent* JetEngineGen2;
	UPROPERTY(EditdefaultsOnly , Category="Components")
	class UBoxComponent* JetEngineControl;
	UPROPERTY(EditdefaultsOnly , Category="Components")
	class UBoxComponent* JetEngineControl2;
	UPROPERTY(EditdefaultsOnly , Category="Components")
	class UBoxComponent* JetFuelStarter;
	UPROPERTY(EditdefaultsOnly , Category="Components")
	class UBoxComponent* JetEngineMaster;
	UPROPERTY(EditdefaultsOnly , Category="Components")
	class UBoxComponent* JetEngineMaster2;
	UPROPERTY(EditdefaultsOnly , Category="Components")
	class UBoxComponent* JetJFSHandle;

	UPROPERTY(EditDefaultsOnly , category="Components")
	class UWidgetComponent* JetWidget;

	UPROPERTY(EditDefaultsOnly , category="Components")
	class UCharacterMovementComponent* movement;
	UPROPERTY(EditDefaultsOnly , category="Components")
	class UNiagaraComponent* BoosterLeftVFX;
	UPROPERTY(EditDefaultsOnly , category="Components")
	class UNiagaraComponent* BoosterRightVFX;

	UPROPERTY(EditDefaultsOnly , Category="Components")
	class USpringArmComponent* JetSprintArm;
	UPROPERTY(EditDefaultsOnly , Category="Components")
	class UCameraComponent* JetCamera;
	UPROPERTY(EditDefaultsOnly , Category="Components")
	class USpringArmComponent* JetSprintArmFPS;
	UPROPERTY(EditDefaultsOnly , Category="Components")
	class UCameraComponent* JetCameraFPS;

	UPROPERTY(EditDefaultsOnly , Category="Components")
	class UPostProcessComponent* JetPostProcess;

public:
	UPROPERTY(EditDefaultsOnly , Category="Components")
	class UArrowComponent* JetFlareArrow1;
	UPROPERTY(EditDefaultsOnly , Category="Components")
	class UArrowComponent* JetFlareArrow2;

private:
	UFUNCTION()
	void OnMyFirstEngineClicked(UPrimitiveComponent* TouchedComponent , struct FKey ButtonPressed);
	UFUNCTION()
	void OnMyMicClicked(UPrimitiveComponent* TouchedComponent , struct FKey ButtonPressed);
	UFUNCTION()
	void OnMyEngineGen1Clicked(UPrimitiveComponent* TouchedComponent , struct FKey ButtonPressed);
	UFUNCTION()
	void OnMyEngineGen2Clicked(UPrimitiveComponent* TouchedComponent , struct FKey ButtonPressed);
	UFUNCTION()
	void OnMyEngineControlClicked(UPrimitiveComponent* TouchedComponent , struct FKey ButtonPressed);
	UFUNCTION()
	void OnMyEngineControl2Clicked(UPrimitiveComponent* TouchedComponent , struct FKey ButtonPressed);
	UFUNCTION()
	void OnMyJetFuelStarterClicked(UPrimitiveComponent* TouchedComponent , struct FKey ButtonPressed);
	UFUNCTION()
	void OnMyEngineMaster1Clicked(UPrimitiveComponent* TouchedComponent , struct FKey ButtonPressed);
	UFUNCTION()
	void OnMyEngineMaster2Clicked(UPrimitiveComponent* TouchedComponent , struct FKey ButtonPressed);
	UFUNCTION()
	void OnMyJFSHandle1Clicked(UPrimitiveComponent* TouchedComponent , struct FKey ButtonPressed);

private: // Input
	UPROPERTY(EditDefaultsOnly , Category="Inputs")
	class UInputMappingContext* IMC_Viper;
	UPROPERTY(EditDefaultsOnly , Category="Inputs")
	class UInputAction* IA_ViperEngine;
	UPROPERTY(EditDefaultsOnly , Category="Inputs")
	class UInputAction* IA_ViperLook;
	UPROPERTY(EditDefaultsOnly , Category="Inputs")
	class UInputAction* IA_ViperZoonIn;
	UPROPERTY(EditDefaultsOnly , Category="Inputs")
	class UInputAction* IA_ViperZoonOut;
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
	UPROPERTY(EditDefaultsOnly , Category="Inputs")
	class UInputAction* IA_ViperChangeWeapon;
	UPROPERTY(EditDefaultsOnly , Category="Inputs")
	class UInputAction* IA_ViperRotateViewTrigger;

	UFUNCTION()
	void F_ViperEngine(const struct FInputActionValue& value);
	UFUNCTION()
	void F_ViperLook(const struct FInputActionValue& value);
	UFUNCTION()
	void F_ViperZoomInStarted(const struct FInputActionValue& value);
	UFUNCTION()
	void F_ViperZoomInCompleted(const struct FInputActionValue& value);
	UFUNCTION()
	void F_ViperZoomOutStarted(const struct FInputActionValue& value);
	UFUNCTION()
	void F_ViperZoomOutCompleted(const struct FInputActionValue& value);
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
	UFUNCTION()
	void F_ViperChangeWeaponStarted(const struct FInputActionValue& value);
	UFUNCTION()
	void F_ViperRotateTriggerStarted(const struct FInputActionValue& value);
	UFUNCTION()
	void F_ViperRotateTriggerCompleted(const struct FInputActionValue& value);

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
	// Zoom
	bool IsZoomIn;
	bool IsZoomOut;

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
	float ZeroGearFlight = -3000.f;
	// 수평비행
	UPROPERTY(EditDefaultsOnly , category = "BasicFlight")
	float BasicFlight50 = 2000.f;
	UPROPERTY(EditDefaultsOnly , category = "BasicFlight")
	float BasicFlight100 = 6000.f;
	UPROPERTY(EditDefaultsOnly , category = "BasicFlight")
	float BasicFlightAB = 8000.f;
	UPROPERTY(EditDefaultsOnly , category = "UpFlight")
	float UpFlight50 = 1000.f;
	UPROPERTY(EditDefaultsOnly , category = "UpFlight")
	float UpFlight100 = 4000.f;
	UPROPERTY(EditDefaultsOnly , category = "UpFlight")
	float UpFlightAB = 6000.f;
	UPROPERTY(EditDefaultsOnly , category = "DownFlight")
	float DownFlight50 = 3000.f;
	UPROPERTY(EditDefaultsOnly , category = "DownFlight")
	float DownFlight100 = 7000.f;
	UPROPERTY(EditDefaultsOnly , category = "DownFlight")
	float DownFlightAB = 9000.f;
	UPROPERTY(EditDefaultsOnly , category = "TurnFlight")
	float TurnFlight50 = 1500.f;
	UPROPERTY(EditDefaultsOnly , category = "TurnFlight")
	float TurnFlight100 = 5500.f;
	UPROPERTY(EditDefaultsOnly , category = "TurnFlight")
	float TurnFlightAB = 6500.f;

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

	UPROPERTY(EditDefaultsOnly , Category="Attack")
	TSubclassOf<class AL_Flare> FlareFactory;

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
	UFUNCTION(Server , Reliable)
	void ServerRPCLocationAndRotation(FVector newLocaction , FRotator newRotator);
	UFUNCTION(NetMulticast , Reliable)
	void MulticastRPCLocationAndRotation(FVector newLocaction , FRotator newRotator);

	UFUNCTION(Server , Reliable)
	void ServerRPCBoost(bool isOn);
	UFUNCTION(NetMulticast , Reliable)
	void MulticastRPCBoost(bool isOn);

	UFUNCTION(Server , Reliable)
	void ServerRPCLockOn();
	UFUNCTION(NetMulticast , Reliable)
	void MulticastRPCLockOn(AActor* target);

	UFUNCTION(Server , Reliable)
	void ServerRPCMissile(AActor* newOwner);
	UFUNCTION(NetMulticast , Reliable)
	void MulticastRPCMissile(AActor* newOwner);

	UFUNCTION(Server , Reliable)
	void ServerRPCFlare(AActor* newOwner);

private:
	UPROPERTY(replicated)
	EWeapon CurrentWeapon = EWeapon::Missile;
	UPROPERTY(replicated , EditDefaultsOnly)
	float FlareCurCnt = 60;

private:
	bool IsRotateTrigger = false;

private:
	UPROPERTY(EditDefaultsOnly)
	float ZoomInValue = 25;
	UPROPERTY(EditDefaultsOnly)
	float ZoomOutValue = 140;

private:
	void CreateDumyComp();

	UPROPERTY(EditDefaultsOnly , Category="DumyComponents")
	class UStaticMeshComponent* DummyMICMesh;
	bool bMIC;

	UPROPERTY(EditDefaultsOnly , Category="DumyComponents")
	class UStaticMeshComponent* DummyEngineGenerMesh1;
	UPROPERTY(EditDefaultsOnly , Category="DumyComponents")
	class UStaticMeshComponent* DummyEngineGenerMesh2;
	bool bEngineGen1;
	bool bEngineGen2;

	UPROPERTY(EditDefaultsOnly , Category="DumyComponents")
	class UStaticMeshComponent* DummyEngineControlMesh1;
	UPROPERTY(EditDefaultsOnly , Category="DumyComponents")
	class UStaticMeshComponent* DummyEngineControlMesh2;
	bool bEngineControl1;
	bool bEngineControl2;

	UPROPERTY(EditDefaultsOnly , Category="DumyComponents")
	class UStaticMeshComponent* DummyJFSMesh;
	bool bJFS;

	UPROPERTY(EditDefaultsOnly , Category="DumyComponents")
	class UStaticMeshComponent* DummyEngineMasterMesh1;
	UPROPERTY(EditDefaultsOnly , Category="DumyComponents")
	class UStaticMeshComponent* DummyEngineMasterMesh2;
	bool bEngineMaster1;
	bool bEngineMaster2;

	UPROPERTY(EditDefaultsOnly , Category="DumyComponents")
	class UStaticMeshComponent* DummyJFSHandleMesh;
	bool bJFSHandle;

	UPROPERTY(EditDefaultsOnly , Category="DumyComponents")
	class UStaticMeshComponent* DummyThrottleMesh;
	bool bThrottleAccel;
	bool bThrottleBreak;
	FVector ThrottleOffLoc = FVector(385 , -28 , 237);
	FVector ThrottleMilLoc = FVector(395 , -28 , 237);
	FVector ThrottleMaxLoc = FVector(405 , -28 , 237);
	UPROPERTY(EditDefaultsOnly, Category="ThrottleSpeed")
	float ThrottleMoveSpeed1 = .15f;
	UPROPERTY(EditDefaultsOnly, Category="ThrottleSpeed")
	float ThrottleMoveSpeed2 = .25f;
	void SetAccelGear();
};
