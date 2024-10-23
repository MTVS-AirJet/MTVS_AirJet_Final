#include "LHJ/L_Viper.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "MTVS_AirJet_Final.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "Camera/CameraComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "LHJ/L_Flare.h"
#include "LHJ/L_HUDWidget.h"
#include "LHJ/L_Missile.h"
#include "Net/UnrealNetwork.h"


#pragma region Construct
AL_Viper::AL_Viper()
{
	PrimaryActorTick.bCanEverTick = true;

	JetRoot = CreateDefaultSubobject<UBoxComponent>(TEXT("JetRoot"));
	RootComponent = JetRoot;
	JetRoot->SetRelativeScale3D(FVector(10.f , 1.f , 2.f));
	JetRoot->SetSimulatePhysics(true);
	JetRoot->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

	// 공기저항
	JetRoot->SetLinearDamping(1.f);
	JetRoot->SetAngularDamping(1.f);

	JetMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("JetMesh"));
	JetMesh->SetupAttachment(JetRoot);
	JetMesh->SetRelativeScale3D(FVector(.1f , 1.f , .5f));
	JetMesh->SetRelativeLocation(FVector(0 , 0 , -28));
	ConstructorHelpers::FObjectFinder<USkeletalMesh> tmpMesh(TEXT(
		"/Script/Engine.SkeletalMesh'/Game/Asset/VigilanteContent/Vehicles/West_Fighter_F18C/SK_West_Fighter_F18C.SK_West_Fighter_F18C'"));
	if (tmpMesh.Succeeded())
	{
		JetMesh->SetSkeletalMesh(tmpMesh.Object);
	}

	JetSprintArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("JetSprintArm"));
	JetSprintArm->SetupAttachment(JetMesh);

	// 3인칭 뷰
	JetSprintArm->SetRelativeLocationAndRotation(FVector(-160 , 0 , 400) , FRotator(-10 , 0 , 0));
	JetSprintArm->TargetArmLength = 2000.f;

	JetSprintArm->bEnableCameraRotationLag = true;
	JetSprintArm->CameraRotationLagSpeed = 3.5f;

	JetCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("JetCamera"));
	JetCamera->SetupAttachment(JetSprintArm);

	JetArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("JetArrow"));
	JetArrow->SetupAttachment(JetMesh);
	JetArrow->SetRelativeLocation(FVector(-1000 , 0 , 0));
	JetArrow->SetHiddenInGame(false); // For Test

	JetFirstEngine = CreateDefaultSubobject<UBoxComponent>(TEXT("JetFirstEngine"));
	JetFirstEngine->SetupAttachment(JetMesh);
	JetFirstEngine->SetRelativeScale3D(FVector(.3f));
	JetFirstEngine->SetRelativeLocation(FVector(390 , -25 , 240));
	JetFirstEngine->SetGenerateOverlapEvents(true);
	JetFirstEngine->OnClicked.AddDynamic(this , &AL_Viper::OnMyFirstEngineClicked);
	JetFirstEngine->SetHiddenInGame(false); // For Test

	JetMic = CreateDefaultSubobject<UBoxComponent>(TEXT("JetMic"));
	JetMic->SetupAttachment(JetMesh);
	JetMic->SetRelativeScale3D(FVector(.1f , .1f , .2f));
	JetMic->SetRelativeLocation(FVector(410 , -15 , 280));
	JetMic->SetGenerateOverlapEvents(true);
	JetMic->OnClicked.AddDynamic(this , &AL_Viper::OnMyMicClicked);
	JetMic->SetHiddenInGame(false); // For Test

	JetEngineMaster = CreateDefaultSubobject<UBoxComponent>(TEXT("JetEngineMaster"));
	JetEngineMaster->SetupAttachment(JetMesh);
	JetEngineMaster->SetRelativeScale3D(FVector(.1f , .1f , .2f));
	JetEngineMaster->SetRelativeLocation(FVector(410 , -5 , 280));
	JetEngineMaster->SetGenerateOverlapEvents(true);
	JetEngineMaster->OnClicked.AddDynamic(this , &AL_Viper::OnMyEngineMasterClicked);
	JetEngineMaster->SetHiddenInGame(false); // For Test

	JetEngineControl = CreateDefaultSubobject<UBoxComponent>(TEXT("JetEngineControl"));
	JetEngineControl->SetupAttachment(JetMesh);
	JetEngineControl->SetRelativeScale3D(FVector(.1f , .1f , .2f));
	JetEngineControl->SetRelativeLocation(FVector(410 , 5 , 280));
	JetEngineControl->SetGenerateOverlapEvents(true);
	JetEngineControl->OnClicked.AddDynamic(this , &AL_Viper::OnMyEngineControlClicked);
	JetEngineControl->SetHiddenInGame(false); // For Test

	JetFuelStarter = CreateDefaultSubobject<UBoxComponent>(TEXT("JetFuelStarter"));
	JetFuelStarter->SetupAttachment(JetMesh);
	JetFuelStarter->SetRelativeScale3D(FVector(.1f , .1f , .2f));
	JetFuelStarter->SetRelativeLocation(FVector(410 , 15 , 280));
	JetFuelStarter->SetGenerateOverlapEvents(true);
	JetFuelStarter->OnClicked.AddDynamic(this , &AL_Viper::OnMyJetFuelStarterClicked);
	JetFuelStarter->SetHiddenInGame(false); // For Test

	JetWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("JetWidget"));
	JetWidget->SetupAttachment(JetMesh);
	JetWidget->SetRelativeLocationAndRotation(FVector(420 , 0 , 295) , FRotator(0 , -180 , 0));
	JetWidget->SetDrawSize(FVector2D(300 , 200));

	movement = CreateDefaultSubobject<UCharacterMovementComponent>(TEXT("movement"));
	//============================================
	JetSprintArmFPS = CreateDefaultSubobject<USpringArmComponent>(TEXT("JetSprintArmFPS"));
	JetSprintArmFPS->SetupAttachment(JetMesh);
	// 조종석 뷰
	JetSprintArmFPS->SetRelativeLocationAndRotation(FVector(350 , 0 , 310) , FRotator(-30 , 0 , 0));
	JetSprintArmFPS->TargetArmLength = 0.f;
	JetSprintArmFPS->bEnableCameraRotationLag = true;
	JetSprintArmFPS->CameraRotationLagSpeed = 3.5f;
	JetCameraFPS = CreateDefaultSubobject<UCameraComponent>(TEXT("JetCameraFPS"));
	JetCameraFPS->SetupAttachment(JetSprintArmFPS);
	JetCameraFPS->SetActive(false);

	//============================================
	BoosterLeftVFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("BoosterLeftVFX"));
	BoosterLeftVFX->SetupAttachment(JetMesh);
	BoosterLeftVFX->SetRelativeLocationAndRotation(FVector(-780 , -50 , 180) , FRotator(0 , 180 , 0));
	ConstructorHelpers::FObjectFinder<UNiagaraSystem> LeftVFX(TEXT(
		"/Script/Niagara.NiagaraSystem'/Game/Asset/RocketThrusterExhaustFX/FX/NS_RocketExhaust_Red.NS_RocketExhaust_Red'"));
	if (LeftVFX.Succeeded())
	{
		BoosterLeftVFX->SetAsset(LeftVFX.Object);
	}

	BoosterRightVFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("BoosterRightVFX"));
	BoosterRightVFX->SetupAttachment(JetMesh);
	BoosterRightVFX->SetRelativeLocationAndRotation(FVector(-780 , 50 , 180) , FRotator(0 , 180 , 0));
	ConstructorHelpers::FObjectFinder<UNiagaraSystem> RightVFX(TEXT(
		"/Script/Niagara.NiagaraSystem'/Game/Asset/RocketThrusterExhaustFX/FX/NS_RocketExhaust_Red.NS_RocketExhaust_Red'"));
	if (RightVFX.Succeeded())
	{
		BoosterRightVFX->SetAsset(RightVFX.Object);
	}

	//============================================
	MissileMoveLoc = CreateDefaultSubobject<USceneComponent>(TEXT("MissileMoveLoc"));
	MissileMoveLoc->SetupAttachment(RootComponent);
	MissileMoveLoc->SetRelativeLocation(FVector(0 , 0 , -200));
	//============================================
	JetFlareArrow1 = CreateDefaultSubobject<UArrowComponent>(TEXT("JetFlareArrow1"));
	JetFlareArrow1->SetupAttachment(JetMesh);
	JetFlareArrow1->SetRelativeLocationAndRotation(FVector(-500 , 100 , 0) , FRotator(-120 , 0 , 0));
	JetFlareArrow1->SetHiddenInGame(false); // For Test
	JetFlareArrow2 = CreateDefaultSubobject<UArrowComponent>(TEXT("JetFlareArrow2"));
	JetFlareArrow2->SetupAttachment(JetMesh);
	JetFlareArrow2->SetRelativeLocationAndRotation(FVector(-500 , -100 , 0) , FRotator(-120 , 0 , 0));
	JetFlareArrow2->SetHiddenInGame(false); // For Test

	bReplicates = true;
	SetReplicateMovement(true);
}
#pragma endregion

void AL_Viper::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AL_Viper , CurrentWeapon);
	DOREPLIFETIME(AL_Viper , FlareMaxCnt);
}

#pragma region Input
// Called to bind functionality to input
void AL_Viper::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* input = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	if (input)
	{
		input->BindAction(IA_ViperEngine , ETriggerEvent::Started , this , &AL_Viper::F_ViperEngine);

		input->BindAction(IA_ViperLook , ETriggerEvent::Triggered , this , &AL_Viper::F_ViperLook);

		input->BindAction(IA_ViperUp , ETriggerEvent::Triggered , this , &AL_Viper::F_ViperUpTrigger);
		input->BindAction(IA_ViperUp , ETriggerEvent::Completed , this , &AL_Viper::F_ViperUpCompleted);

		input->BindAction(IA_ViperDown , ETriggerEvent::Triggered , this , &AL_Viper::F_ViperDownTrigger);
		input->BindAction(IA_ViperDown , ETriggerEvent::Completed , this , &AL_Viper::F_ViperDownCompleted);

		input->BindAction(IA_ViperRight , ETriggerEvent::Triggered , this , &AL_Viper::F_ViperRightTrigger);
		input->BindAction(IA_ViperRight , ETriggerEvent::Completed , this , &AL_Viper::F_ViperRightCompleted);

		input->BindAction(IA_ViperLeft , ETriggerEvent::Triggered , this , &AL_Viper::F_ViperLeftTrigger);
		input->BindAction(IA_ViperLeft , ETriggerEvent::Completed , this , &AL_Viper::F_ViperLeftCompleted);

		input->BindAction(IA_ViperTurnRight , ETriggerEvent::Triggered , this , &AL_Viper::F_ViperTurnRightTrigger);
		input->BindAction(IA_ViperTurnRight , ETriggerEvent::Completed , this , &AL_Viper::F_ViperTurnRightCompleted);

		input->BindAction(IA_ViperTurnLeft , ETriggerEvent::Triggered , this , &AL_Viper::F_ViperTurnLeftTrigger);
		input->BindAction(IA_ViperTurnLeft , ETriggerEvent::Completed , this , &AL_Viper::F_ViperTurnLeftCompleted);

		input->BindAction(IA_ViperAccel , ETriggerEvent::Started , this , &AL_Viper::F_ViperAccelStarted);
		input->BindAction(IA_ViperAccel , ETriggerEvent::Completed , this , &AL_Viper::F_ViperAccelCompleted);

		input->BindAction(IA_ViperBreak , ETriggerEvent::Started , this , &AL_Viper::F_ViperBreakStarted);
		input->BindAction(IA_ViperBreak , ETriggerEvent::Completed , this , &AL_Viper::F_ViperBreakCompleted);

		input->BindAction(IA_ViperResetRotation , ETriggerEvent::Started , this , &AL_Viper::F_ViperResetRotation);

		input->BindAction(IA_ViperShoot , ETriggerEvent::Started , this , &AL_Viper::F_ViperShootStarted);

		input->BindAction(IA_ViperFPS , ETriggerEvent::Started , this , &AL_Viper::F_ViperFpsStarted);
		input->BindAction(IA_ViperTPS , ETriggerEvent::Started , this , &AL_Viper::F_ViperTpsStarted);

		input->BindAction(IA_ViperChangeWeapon , ETriggerEvent::Started , this , &AL_Viper::F_ViperChangeWeaponStarted);
	}
}

void AL_Viper::OnMyFirstEngineClicked(UPrimitiveComponent* TouchedComponent , FKey ButtonPressed)
{
	if (!bFirstEngine)
	{
		LOG_SCREEN("Clicked!!");
		AccelGear = 1;
		bFirstEngine = true;
	}
}

void AL_Viper::OnMyMicClicked(UPrimitiveComponent* TouchedComponent , struct FKey ButtonPressed)
{
	LOG_SCREEN("MIC 클릭");
}

void AL_Viper::OnMyEngineMasterClicked(UPrimitiveComponent* TouchedComponent , struct FKey ButtonPressed)
{
	LOG_SCREEN("EngineMaster 클릭");
}

void AL_Viper::OnMyEngineControlClicked(UPrimitiveComponent* TouchedComponent , struct FKey ButtonPressed)
{
	LOG_SCREEN("EngineControl 클릭");
}

void AL_Viper::OnMyJetFuelStarterClicked(UPrimitiveComponent* TouchedComponent , struct FKey ButtonPressed)
{
	LOG_SCREEN("JFS 클릭");
}

void AL_Viper::F_ViperEngine(const FInputActionValue& value)
{
	bool b = value.Get<bool>();
	IsEngineOn = !IsEngineOn;
	LOG_SCREEN("%s" , IsEngineOn?TEXT("True"):TEXT("false"));
}

void AL_Viper::F_ViperLook(const FInputActionValue& value)
{
	auto v = value.Get<FVector2D>();
	// AddControllerYawInput(v.X);
	// AddControllerPitchInput(v.Y);
	if (JetCamera->IsActive())
	{
		FRotator FPSrot = JetSprintArm->GetRelativeRotation();
		float newFpsYaw = FPSrot.Yaw + v.X;
		float newFpsPitch = FPSrot.Pitch + v.Y;
		newFpsYaw = UKismetMathLibrary::FClamp(newFpsYaw , -360.f , 360.f);
		newFpsPitch = UKismetMathLibrary::FClamp(newFpsPitch , -100.f , 100.f);
		JetSprintArm->SetRelativeRotation(FRotator(newFpsPitch , newFpsYaw , 0));
	}
	else
	{
		FRotator FPSrot = JetSprintArmFPS->GetRelativeRotation();
		float newFpsYaw = FPSrot.Yaw + v.X;
		float newFpsPitch = FPSrot.Pitch + v.Y;
		newFpsYaw = UKismetMathLibrary::FClamp(newFpsYaw , -150.f , 150.f);
		newFpsPitch = UKismetMathLibrary::FClamp(newFpsPitch , -100.f , 100.f);
		JetSprintArmFPS->SetRelativeRotation(FRotator(newFpsPitch , newFpsYaw , 0));
	}
}

void AL_Viper::F_ViperUpTrigger(const FInputActionValue& value)
{
	IsKeyUpPress = true;

	if (CurrentTime < ChangeTime)
		return;

	CurrentTime = 0.f;
	ForceUnitRot = CombineRotate(ChangeMoveVector);
}

void AL_Viper::F_ViperUpCompleted(const FInputActionValue& value)
{
	IsKeyUpPress = false;
	ForceUnitRot = FRotator(0 , 0 , 0);
}

void AL_Viper::F_ViperDownTrigger(const FInputActionValue& value)
{
	IsKeyDownPress = true;

	if (CurrentTime < ChangeTime)
		return;

	CurrentTime = 0.f;
	ForceUnitRot = CombineRotate(-1 * ChangeMoveVector);
}

void AL_Viper::F_ViperDownCompleted(const FInputActionValue& value)
{
	IsKeyDownPress = false;
	ForceUnitRot = FRotator(0 , 0 , 0);
}

void AL_Viper::F_ViperRightTrigger(const FInputActionValue& value)
{
	IsKeyRightPress = true;
}

void AL_Viper::F_ViperRightCompleted(const FInputActionValue& value)
{
	IsKeyRightPress = false;
}

void AL_Viper::F_ViperLeftTrigger(const FInputActionValue& value)
{
	IsKeyLeftPress = true;
}

void AL_Viper::F_ViperLeftCompleted(const FInputActionValue& value)
{
	IsKeyLeftPress = false;
}

void AL_Viper::F_ViperTurnRightTrigger(const FInputActionValue& value)
{
	IsRightRoll = true;
}

void AL_Viper::F_ViperTurnRightCompleted(const FInputActionValue& value)
{
	IsRightRoll = false;
}

void AL_Viper::F_ViperTurnLeftTrigger(const FInputActionValue& value)
{
	IsLeftRoll = true;
}

void AL_Viper::F_ViperTurnLeftCompleted(const FInputActionValue& value)
{
	IsLeftRoll = false;
}

void AL_Viper::F_ViperResetRotation(const FInputActionValue& value)
{
	SetActorRotation(FRotator(0 , 0 , 0));
	SetActorRelativeRotation(FRotator(0 , 0 , 0));
	JetArrow->SetRelativeRotation(FRotator(0 , 0 , 0));
}

void AL_Viper::F_ViperAccelStarted(const FInputActionValue& value)
{
	AccelGear++;
	if (AccelGear > 3)
		AccelGear = 3;
	IsAccel = true;
}

void AL_Viper::F_ViperAccelCompleted(const FInputActionValue& value)
{
	KeyDownAccel = 0.f;
	IsAccel = false;
}

void AL_Viper::F_ViperBreakStarted(const FInputActionValue& value)
{
	AccelGear--;
	if (AccelGear < 0)
		AccelGear = 0;
	IsBreak = true;
}

void AL_Viper::F_ViperBreakCompleted(const FInputActionValue& value)
{
	KeyDownAccel = 0.f;
	IsBreak = false;
}

void AL_Viper::F_ViperShootStarted(const struct FInputActionValue& value)
{
	if (CurrentWeapon == EWeapon::Missile)
		ServerRPCMissile(this);
	else if (CurrentWeapon == EWeapon::Flare)
		ServerRPCFlare(this);
}

void AL_Viper::F_ViperFpsStarted(const struct FInputActionValue& value)
{
	if (JetCamera)
		JetCamera->SetActive(false);
	if (JetCameraFPS)
		JetCameraFPS->SetActive(true);
}

void AL_Viper::F_ViperTpsStarted(const struct FInputActionValue& value)
{
	if (JetCamera)
		JetCamera->SetActive(true);
	if (JetCameraFPS)
		JetCameraFPS->SetActive(false);
}

void AL_Viper::F_ViperChangeWeaponStarted(const struct FInputActionValue& value)
{
	// 현재 값에서 1을 더하고, Max로 나눈 나머지를 사용하여 순환
	CurrentWeapon = static_cast<EWeapon>((static_cast<int32>(CurrentWeapon) + 1) % static_cast<int32>(EWeapon::Max));
}

FRotator AL_Viper::CombineRotate(FVector NewVector)
{
	FRotator loc_rot = FRotator(NewVector.Y , NewVector.X , NewVector.Z);
	return FRotator(ForceUnitRot.Pitch + loc_rot.Pitch , ForceUnitRot.Yaw + loc_rot.Yaw ,
	                ForceUnitRot.Roll + loc_rot.Roll);
}
#pragma endregion

void AL_Viper::BeginPlay()
{
	Super::BeginPlay();

	auto pc = Cast<APlayerController>(Controller);
	if (pc)
	{
		UEnhancedInputLocalPlayerSubsystem* subsys = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
			pc->GetLocalPlayer());
		if (subsys)
		{
			subsys->AddMappingContext(IMC_Viper , 0);
		}

		pc->bEnableClickEvents = true;
	}

	JetWidget->SetWidgetClass(HUD_UI);
}

void AL_Viper::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	PrintNetLog();

	CurrentTime += DeltaTime;

#pragma region Rotate JetArrow
	FRotator jetRot = JetArrow->GetRelativeRotation();
	// Check Distance Into Area
	if (IsKeyUpPress || IsKeyDownPress)
	{
		if (jetRot.Pitch > MaxPitchValue)
		{
			JetArrow->SetRelativeRotation(FRotator(MaxPitchValue - 1.f , jetRot.Yaw , jetRot.Roll));
			ForceUnitRot = FRotator(0 , 0 , 0);
		}
		else if (jetRot.Pitch < MinPitchValue)
		{
			JetArrow->SetRelativeRotation(FRotator(MinPitchValue + 1.f , jetRot.Yaw , jetRot.Roll));
			ForceUnitRot = FRotator(0 , 0 , 0);
		}
		else
		{
			JetArrow->AddRelativeRotation(FRotator(ForceUnitRot.Pitch , 0 , 0));
		}
	}

	if (IsKeyLeftPress)
	{
		if (!IsKeyRightPress)
		{
			FRotator resetRot = FRotator(jetRot.Pitch , 0 , jetRot.Roll);
			JetArrow->SetRelativeRotation(resetRot);
			FRotator newRot = FRotator(jetRot.Pitch , MinYawValue , jetRot.Roll);
			JetArrow->AddRelativeRotation(newRot);
		}
	}
	else if (IsKeyRightPress)
	{
		if (!IsKeyLeftPress)
		{
			FRotator resetRot = FRotator(jetRot.Pitch , 0 , jetRot.Roll);
			JetArrow->SetRelativeRotation(resetRot);
			FRotator newRot = FRotator(jetRot.Pitch , MaxYawValue , jetRot.Roll);
			JetArrow->AddRelativeRotation(newRot);
		}
	}

	// 방향전환중이 아니라면 방향을 가운데로 변환
	if (!IsKeyLeftPress && !IsKeyRightPress)
		JetArrow->SetRelativeRotation(FRotator(JetArrow->GetRelativeRotation().Pitch , 0 ,
		                                       JetArrow->GetRelativeRotation().Roll));

	//LOG_SCREEN("현재 각도는 %f 입니다." , JetArrow->GetRelativeRotation().Pitch);
#pragma endregion

#pragma region Change Accel Value
	if (IsAccel)
	{
		KeyDownAccel += DeltaTime;
		if (KeyDownAccel >= ChangeAccelTime)
		{
			KeyDownAccel = 0.f;

			// 기어 변경
			AccelGear++;
			if (AccelGear > 3)
				AccelGear = 3;
		}
	}
	else if (IsBreak)
	{
		KeyDownAccel += DeltaTime;
		if (KeyDownAccel >= ChangeAccelTime)
		{
			KeyDownAccel = 0.f;

			// 기어 변경
			AccelGear--;
			if (AccelGear < 0)
				AccelGear = 0;
		}
	}
#pragma endregion

#pragma region Jet Move
	ValueOfMoveForce += GetAddTickSpeed();
	if (ValueOfMoveForce < 0)
		ValueOfMoveForce = 0;
	else if (ValueOfMoveForce > MaxValueOfMoveForce)
		ValueOfMoveForce = MaxValueOfMoveForce;

	if (IsEngineOn)
	{
		// Add Force
		// LOG_S(Warning , TEXT("======================="));
		FVector forceVec = JetArrow->GetForwardVector() * ValueOfMoveForce;
		FVector forceLoc = JetRoot->GetComponentLocation();
		// LOG_S(Warning , TEXT("forceLoc x : %f, y : %f, z : %f") , forceLoc.X , forceLoc.Y , forceLoc.Z);
		if (JetRoot->IsSimulatingPhysics())
			JetRoot->AddForceAtLocation(forceVec , forceLoc);

		// Move Up & Down
		jetRot = JetArrow->GetRelativeRotation();
		float zRot = jetRot.Quaternion().Y * jetRot.Quaternion().W * ValueOfHeightForce * 10.f;
		//LOG_S(Warning , TEXT("zRot %f") , zRot);
		JetRoot->AddForceAtLocation(FVector(0 , 0 , zRot) , HeightForceLoc);


		// Rotate
		jetRot = JetArrow->GetRelativeRotation();
		//LOG_S(Warning , TEXT("Rotate Yaw %f") , jetRot.Yaw / ValueOfDivRot);
		JetRoot->AddRelativeRotation(FRotator(0 , jetRot.Yaw / ValueOfDivRot , 0));

		if (IsLeftRoll)
			JetRoot->AddRelativeRotation(-1 * RotateValue);
		if (IsRightRoll)
			JetRoot->AddRelativeRotation(RotateValue);

		if (IsLocallyControlled())
		{
			ServerRPCLocationAndRotation(JetRoot->GetComponentLocation() , JetRoot->GetRelativeRotation());
		}
	}
	JetArrow->SetRelativeRotation(FRotator(0 , 0 , 0));
#pragma endregion

	if (IsLocallyControlled())
	{
#pragma region 고도계
		float CurrHeight = GetActorLocation().Z + HeightOfSea; // 고도 높이
		float CurrFeet = CurrHeight / 30.48; // cm to feet
		if (CurrFeet < 0)
			CurrFeet = 0;

		if (auto HUDui = Cast<UL_HUDWidget>(JetWidget->GetWidget()))
		{
			HUDui->UpdateHeightBar(CurrFeet);
		}
#pragma endregion

#pragma region 속도계
		int32 ValueOfMoveForceInNote = static_cast<int32>(ValueOfMoveForce / 1650.0);
		if (auto HUDui = Cast<UL_HUDWidget>(JetWidget->GetWidget()))
		{
			HUDui->UpdateHeightText(ValueOfMoveForceInNote);
		}
#pragma endregion

#pragma region LockOn
		IsLockOn();
#pragma endregion

		ChangeBooster();
	}

#pragma region Flare Arrow Rotation Change
	if (CurrentWeapon == EWeapon::Flare)
	{
		int32 randRot = FMath::RandRange(-150 , -110);
		LOG_SCREEN("%d", randRot);
		FRotator newFlareRot = FRotator(randRot , 0 , 0);
		JetFlareArrow1->SetRelativeRotation(newFlareRot);
		JetFlareArrow2->SetRelativeRotation(newFlareRot);
	}
#pragma endregion
}

#pragma region Get Force
float AL_Viper::GetAddTickSpeed()
{
	float fRtn = 0.f;

	if (AccelGear == 0)
		fRtn = ZeroGearFlight;
	else
	{
		FRotator jetRot = JetArrow->GetRelativeRotation();
		// 수평비행
		if (jetRot == FRotator(0 , 0 , 0))
		{
			if (AccelGear == 1)
				fRtn = BasicFlight50;
			else if (AccelGear == 2)
				fRtn = BasicFlight100;
			else if (AccelGear == 3)
				fRtn = BasicFlightAB;
		}
		// 상승비행
		else if (jetRot.Pitch > 0)
		{
			if (AccelGear == 1)
				fRtn = UpFlight50;
			else if (AccelGear == 2)
				fRtn = UpFlight100;
			else if (AccelGear == 3)
				fRtn = UpFlightAB;
		}
		// 하강비행
		else if (jetRot.Pitch < 0)
		{
			if (AccelGear == 1)
				fRtn = DownFlight50;
			else if (AccelGear == 2)
				fRtn = DownFlight100;
			else if (AccelGear == 3)
				fRtn = DownFlightAB;
		}
		// 좌우회전
		else if (jetRot.Yaw > 0 || jetRot.Yaw < 0)
		{
			if (AccelGear == 1)
				fRtn = TurnFlight50;
			else if (AccelGear == 2)
				fRtn = TurnFlight100;
			else if (AccelGear == 3)
				fRtn = TurnFlightAB;
		}
	}

	return fRtn;
}

void AL_Viper::IsLockOn()
{
	ServerRPCLockOn();
}
#pragma endregion

void AL_Viper::ChangeBooster()
{
	if (IsEngineOn && AccelGear == 3)
	{
		ServerRPCBoost(true);
	}
	else
	{
		ServerRPCBoost(false);
	}
}

void AL_Viper::PrintNetLog()
{
	const FString conStr = GetNetConnection() ? TEXT("Valid Connection") : TEXT("Invalid Connection");
	const FString ownerName = GetOwner() ? GetOwner()->GetName() : TEXT("No Owner");

	FString logStr = FString::Printf(
		TEXT("Connection : %s\nOwner Name : %s\nLocal Role : %s\nRemote Role : %s") , *conStr , *ownerName ,
		*LOCALROLE , *REMOTEROLE);
	FVector loc = GetActorLocation() + GetActorUpVector() * 30;
	DrawDebugString(GetWorld() , loc , logStr , nullptr , FColor::Yellow , 0 , true , 1.f);
}

void AL_Viper::ServerRPCBoost_Implementation(bool isOn)
{
	MulticastRPCBoost(isOn);
}

void AL_Viper::MulticastRPCBoost_Implementation(bool isOn)
{
	if (isOn)
	{
		if (BoosterLeftVFX)
		{
			BoosterLeftVFX->SetVariableFloat(FName("EnergyCore_Life") , 0.7f);
			BoosterLeftVFX->SetVariableFloat(FName("HeatHaze_Lifetime") , .1f);
			BoosterLeftVFX->SetVariableFloat(FName("Particulate_Life") , 0.05f);
			BoosterLeftVFX->SetVariableFloat(FName("Thrusters_Life") , 0.1f);
		}
		if (BoosterRightVFX)
		{
			BoosterRightVFX->SetVariableFloat(FName("EnergyCore_Life") , 0.7f);
			BoosterRightVFX->SetVariableFloat(FName("HeatHaze_Lifetime") , .1f);
			BoosterRightVFX->SetVariableFloat(FName("Particulate_Life") , 0.05f);
			BoosterRightVFX->SetVariableFloat(FName("Thrusters_Life") , 0.1f);
		}
	}
	else
	{
		// 엔진부스터 끄기
		if (BoosterLeftVFX)
		{
			BoosterLeftVFX->SetVariableFloat(FName("EnergyCore_Life") , 0.f);
			BoosterLeftVFX->SetVariableFloat(FName("HeatHaze_Lifetime") , 0.f);
			BoosterLeftVFX->SetVariableFloat(FName("Particulate_Life") , 0.f);
			BoosterLeftVFX->SetVariableFloat(FName("Thrusters_Life") , 0.f);
		}
		if (BoosterRightVFX)
		{
			BoosterRightVFX->SetVariableFloat(FName("EnergyCore_Life") , 0.f);
			BoosterRightVFX->SetVariableFloat(FName("HeatHaze_Lifetime") , 0.f);
			BoosterRightVFX->SetVariableFloat(FName("Particulate_Life") , 0.f);
			BoosterRightVFX->SetVariableFloat(FName("Thrusters_Life") , 0.f);
		}
	}
}

void AL_Viper::ServerRPCLocationAndRotation_Implementation(FVector newLocaction , FRotator newRotator)
{
	MulticastRPCLocationAndRotation(newLocaction , newRotator);
}

void AL_Viper::MulticastRPCLocationAndRotation_Implementation(FVector newLocaction , FRotator newRotator)
{
	if (!IsLocallyControlled())
	{
		SetActorLocation(newLocaction);
		SetActorRotation(newRotator);
	}
}

void AL_Viper::ServerRPCMissile_Implementation(AActor* newOwner)
{
	if (LockOnTarget)
	{
		if (Missile)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = newOwner;
			FRotator SpawnRotation = FRotator::ZeroRotator; // Update this with the desired rotation for the missile
			FVector SpawnLocation = GetActorLocation(); // Update this with the desired location for the missile

			AL_Missile* SpawnedMissile = GetWorld()->SpawnActor<AL_Missile>(
				Missile , SpawnLocation , SpawnRotation , SpawnParams);
			if (SpawnedMissile)
			{
				// Optionally add any initialization for the spawned missile here
				LOG_S(Warning , TEXT("미사일 발사!! 타겟은 %s") , *LockOnTarget->GetName());
			}
		}
		else
		{
			LOG_S(Warning , TEXT("미사일 액터가 없습니다."));
		}
	}
	else
	{
		LOG_S(Warning , TEXT("타겟이 없습니다!!"));
	}
	//MulticastRPCMissile(newOwner);
}

void AL_Viper::MulticastRPCMissile_Implementation(AActor* newOwner)
{
}

void AL_Viper::ServerRPCFlare_Implementation(AActor* newOwner)
{
	if (FlareFactory)
	{
		if(FlareCurCnt>0)
		{
			// 던질 위치 계산(캐릭터 위치에서 위로 조정)
			FVector SpawnLocation = JetFlareArrow1->GetComponentLocation();
			// 던질 각도
			FRotator SpawnRotation = JetMesh->GetComponentRotation();
			// FlareFactory 이용해서 수류탄 스폰
			AL_Flare* Flare1 = GetWorld()->SpawnActor<AL_Flare>(FlareFactory , SpawnLocation , SpawnRotation);

			if (Flare1)
			{
				Flare1->SetOwner(newOwner);
				FlareCurCnt--;
			}

			// 던질 위치 계산(캐릭터 위치에서 위로 조정)
			SpawnLocation = JetFlareArrow2->GetComponentLocation();
			// 던질 각도
			//SpawnRotation = FRotator::ZeroRotator;
			// FlareFactory 이용해서 수류탄 스폰
			AL_Flare* Flare2 = GetWorld()->SpawnActor<AL_Flare>(FlareFactory , SpawnLocation , SpawnRotation);

			if (Flare2)
			{
				Flare2->SetOwner(newOwner);
				FlareCurCnt--;
			}
		}
		else
		{
			LOG_S(Warning , TEXT("남은 Flare가 없습니다."));
		}
	}
	else
	{
		LOG_S(Warning , TEXT("FlareFactory가 없습니다."));
	}
}

void AL_Viper::ServerRPCLockOn_Implementation()
{
	LockOnTarget = nullptr;
	FVector Start = JetMesh->GetComponentLocation();
	FVector ForwardVector = JetMesh->GetForwardVector();

	TArray<AActor*> Overlaps;
	TArray<FHitResult> OutHit;
	for (int i = 0; i < RangeCnt; i++)
	{
		Diametr *= 2.f;
		Start += (ForwardVector * Diametr / 2);
		if (UKismetSystemLibrary::SphereTraceMulti(GetWorld() , Start , Start , Diametr / 2.f , TraceTypeQuery1 ,
		                                           false , Overlaps , EDrawDebugTrace::ForOneFrame , OutHit , true))
		{
			for (auto hit : OutHit)
			{
				if (hit.GetActor()->ActorHasTag("target"))
				{
					MulticastRPCLockOn(hit.GetActor());
				}
			}
		}
	}

	Diametr = 30.f;
}

void AL_Viper::MulticastRPCLockOn_Implementation(AActor* target)
{
	LockOnTarget = target;
	// LOG_S(Warning , TEXT("Viper Name : %s, LockOnTarget Name : %s") , *GetName() ,
	// 					  *LockOnTarget->GetName());
}
