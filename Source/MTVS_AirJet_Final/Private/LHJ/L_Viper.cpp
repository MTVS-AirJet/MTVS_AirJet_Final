#include "LHJ/L_Viper.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/SpringArmComponent.h"


#pragma region Construct
AL_Viper::AL_Viper()
{
	PrimaryActorTick.bCanEverTick = true;

	JetMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("JetMesh"));
	RootComponent = JetMesh;
	ConstructorHelpers::FObjectFinder<USkeletalMesh> tmpMesh(TEXT(
		"/Script/Engine.SkeletalMesh'/Game/VigilanteContent/Vehicles/West_Fighter_F18C/SK_West_Fighter_F18C.SK_West_Fighter_F18C'"));
	if (tmpMesh.Succeeded())
	{
		JetMesh->SetSkeletalMesh(tmpMesh.Object);
	}

	JetSprintArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("JetSprintArm"));
	JetSprintArm->SetupAttachment(RootComponent);
	JetSprintArm->SetRelativeLocationAndRotation(FVector(-160 , 0 , 400) , FRotator(-10 , 0 , 0));
	JetSprintArm->TargetArmLength = 2000.f;
	JetSprintArm->bEnableCameraRotationLag = true;
	JetSprintArm->CameraRotationLagSpeed = 3.5f;

	JetCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("JetCamera"));
	JetCamera->SetupAttachment(JetSprintArm);

	JetArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("JetArrow"));
	JetArrow->SetupAttachment(RootComponent);
	JetArrow->SetRelativeLocation(FVector(-1000 , 0 , 0));
	JetArrow->SetHiddenInGame(false); // For Test
}
#pragma endregion

#pragma region Input
// Called to bind functionality to input
void AL_Viper::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* input = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	if (input)
	{
		input->BindAction(IA_ViperEngine , ETriggerEvent::Triggered , this , &AL_Viper::F_ViperEngine);
		input->BindAction(IA_ViperLook , ETriggerEvent::Triggered , this , &AL_Viper::F_ViperLook);
		input->BindAction(IA_ViperUp , ETriggerEvent::Triggered , this , &AL_Viper::F_ViperUpTrigger);
		input->BindAction(IA_ViperUp , ETriggerEvent::Completed , this , &AL_Viper::F_ViperUpCompleted);
		input->BindAction(IA_ViperDown , ETriggerEvent::Triggered , this , &AL_Viper::F_ViperDownTrigger);
		input->BindAction(IA_ViperDown , ETriggerEvent::Completed , this , &AL_Viper::F_ViperDownCompleted);
		input->BindAction(IA_ViperRight , ETriggerEvent::Triggered , this , &AL_Viper::F_ViperRightTrigger);
		input->BindAction(IA_ViperRight , ETriggerEvent::Completed , this , &AL_Viper::F_ViperRightCompleted);
		input->BindAction(IA_ViperLeft , ETriggerEvent::Triggered , this , &AL_Viper::F_ViperLeftTrigger);
		input->BindAction(IA_ViperLeft , ETriggerEvent::Completed , this , &AL_Viper::F_ViperLeftCompleted);
	}
}

void AL_Viper::F_ViperEngine(const FInputActionValue& value)
{
	IsEngineOn = value.Get<bool>();
}

void AL_Viper::F_ViperLook(const FInputActionValue& value)
{
}

void AL_Viper::F_ViperUpTrigger(const FInputActionValue& value)
{
	FVector inputVec = value.Get<FVector>();

	if (CurrentTime < ChangeTime)
		return;

	CurrentTime = 0.f;
	ForceUnitRot = CombineRotate(inputVec);
	IsKeyUpPress = true;
}

void AL_Viper::F_ViperUpCompleted(const FInputActionValue& value)
{
	FVector inputVec = value.Get<FVector>();
	ForceUnitRot = CombineRotate(-inputVec);
	IsKeyUpPress = false;
}

void AL_Viper::F_ViperDownTrigger(const FInputActionValue& value)
{
	FVector inputVec = value.Get<FVector>();

	if (CurrentTime < ChangeTime)
		return;

	CurrentTime = 0.f;
	ForceUnitRot = CombineRotate(inputVec);
	IsKeyDownPress = true;
}

void AL_Viper::F_ViperDownCompleted(const FInputActionValue& value)
{
	FVector inputVec = value.Get<FVector>();
	ForceUnitRot = CombineRotate(-inputVec);
	IsKeyDownPress = false;
}

void AL_Viper::F_ViperRightTrigger(const FInputActionValue& value)
{
	FVector inputVec = value.Get<FVector>();

	if (CurrentTime < ChangeTime)
		return;

	CurrentTime = 0.f;
	ForceUnitRot = CombineRotate(inputVec);
	IsKeyRightPress = true;
}

void AL_Viper::F_ViperRightCompleted(const FInputActionValue& value)
{
	FVector inputVec = value.Get<FVector>();
	ForceUnitRot = CombineRotate(-inputVec);
	IsKeyRightPress = false;
}

void AL_Viper::F_ViperLeftTrigger(const FInputActionValue& value)
{
	FVector inputVec = value.Get<FVector>();

	if (CurrentTime < ChangeTime)
		return;

	CurrentTime = 0.f;
	ForceUnitRot = CombineRotate(inputVec);
	IsKeyLeftPress = true;
}

void AL_Viper::F_ViperLeftCompleted(const FInputActionValue& value)
{
	FVector inputVec = value.Get<FVector>();
	ForceUnitRot = CombineRotate(-inputVec);
	IsKeyLeftPress = false;
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
	}
}

void AL_Viper::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Add DeltaTime To CurrentTime
	CurrentTime += DeltaTime;

#pragma region Rotate JetArrow
	// Check Distance Into Area
	FRotator jetRot = JetArrow->GetRelativeRotation();
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

		jetRot = JetArrow->GetRelativeRotation();
		if (jetRot.Yaw > MaxYawValue)
		{
			JetArrow->SetRelativeRotation(FRotator(jetRot.Pitch , jetRot.Yaw , MaxYawValue - 1.f));
			ForceUnitRot = FRotator(0 , 0 , 0);
		}
		else if (jetRot.Yaw < MinYawValue)
		{
			JetArrow->SetRelativeRotation(FRotator(jetRot.Pitch , jetRot.Yaw , MinYawValue + 1.f));
			ForceUnitRot = FRotator(0 , 0 , 0);
		}
		else
		{
			JetArrow->AddRelativeRotation(FRotator(0 , ForceUnitRot.Yaw , 0));
		}
	}
#pragma endregion

#pragma region Jet Move
	if (IsEngineOn)
	{
		// Add Force
		FVector forceVec = JetArrow->GetForwardVector() * ValueOfMoveForce;
		FVector forceLoc = JetMesh->GetRelativeLocation();
		JetMesh->AddForceAtLocation(forceVec , forceLoc);

		// Move Up & Down
		jetRot = JetArrow->GetRelativeRotation();
		JetMesh->AddForceAtLocation(FVector(0 , 0 , jetRot.Pitch * ValueOfHeightForce) , HeightForceLoc);

		// Rotate
		jetRot = JetArrow->GetRelativeRotation();
		JetMesh->AddRelativeRotation(FRotator(0 , jetRot.Yaw / ValueOfDivRot , 0));
	}
#pragma endregion

#pragma region Reset Jet Arrow
	if (IsKeyUpPress || IsKeyDownPress)
	{
		jetRot = JetArrow->GetRelativeRotation();
		ForceUnitRot = FRotator(jetRot.Pitch * ValueOfArrowReset , ForceUnitRot.Yaw , ForceUnitRot.Roll);
		JetArrow->AddRelativeRotation(ForceUnitRot);
	}

	if (IsKeyRightPress || IsKeyLeftPress)
	{
		jetRot = JetArrow->GetRelativeRotation();
		ForceUnitRot = FRotator(ForceUnitRot.Pitch , jetRot.Yaw * ValueOfArrowReset , ForceUnitRot.Roll);
		JetArrow->AddRelativeRotation(ForceUnitRot);
	}
#pragma endregion
}