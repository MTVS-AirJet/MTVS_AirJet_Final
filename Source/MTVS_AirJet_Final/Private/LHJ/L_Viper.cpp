#include "LHJ/L_Viper.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "MTVS_AirJet_Final.h"
#include "Camera/CameraComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "LHJ/L_HUDWidget.h"


#pragma region Construct
AL_Viper::AL_Viper()
{
	PrimaryActorTick.bCanEverTick = true;

	JetRoot = CreateDefaultSubobject<UBoxComponent>(TEXT("JetRoot"));
	RootComponent = JetRoot;
	JetRoot->SetSimulatePhysics(true);
	JetRoot->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

	// 공기저항
	JetRoot->SetLinearDamping(1.f);
	JetRoot->SetAngularDamping(1.f);

	JetMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("JetMesh"));
	JetMesh->SetupAttachment(JetRoot);
	ConstructorHelpers::FObjectFinder<USkeletalMesh> tmpMesh(TEXT(
		"/Script/Engine.SkeletalMesh'/Game/Asset/VigilanteContent/Vehicles/West_Fighter_F18C/SK_West_Fighter_F18C.SK_West_Fighter_F18C'"));
	if (tmpMesh.Succeeded())
	{
		JetMesh->SetSkeletalMesh(tmpMesh.Object);
	}

	JetSprintArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("JetSprintArm"));
	JetSprintArm->SetupAttachment(JetMesh);

	// 3인칭 뷰
	//JetSprintArm->SetRelativeLocationAndRotation(FVector(-160 , 0 , 400) , FRotator(-10 , 0 , 0));
	//JetSprintArm->TargetArmLength = 2000.f;

	// 조종석 뷰
	JetSprintArm->SetRelativeLocationAndRotation(FVector(350 , 0 , 310) , FRotator(0 , -30 , 0));
	JetSprintArm->TargetArmLength = 0.f;

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
	//JetFirstEngine->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	JetFirstEngine->SetHiddenInGame(false); // For Test

	JetWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("JetWidget"));
	JetWidget->SetupAttachment(JetMesh);
	JetWidget->SetRelativeLocationAndRotation(FVector(420 , 0 , 295) , FRotator(0 , -180 , 0));
	JetWidget->SetDrawSize(FVector2D(300 , 200));

	movement = CreateDefaultSubobject<UCharacterMovementComponent>(TEXT("movement"));
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
	}
}

void AL_Viper::OnMyFirstEngineClicked(UPrimitiveComponent* TouchedComponent , FKey ButtonPressed)
{
	if (!bFristEngine)
	{
		LOG_SCREEN("Clicked!!");
		AccelGear = 1;
		bFristEngine = true;
	}
}

void AL_Viper::F_ViperEngine(const FInputActionValue& value)
{
	bool b = value.Get<bool>();
	IsEngineOn = !IsEngineOn;
	LOG_SCREEN("%s" , IsEngineOn?TEXT("True"):TEXT("false"));
}

void AL_Viper::F_ViperLook(const FInputActionValue& value)
{
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

	ValueOfMoveForce += GetAddTickSpeed();
	if (ValueOfMoveForce < 0)
		ValueOfMoveForce = 0;
	else if (ValueOfMoveForce > MaxValueOfMoveForce)
		ValueOfMoveForce = MaxValueOfMoveForce;
#pragma endregion

#pragma region Jet Move
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
		LOG_S(Warning , TEXT("zRot %f") , zRot);
		JetRoot->AddForceAtLocation(FVector(0 , 0 , zRot) , HeightForceLoc);


		// Rotate
		jetRot = JetArrow->GetRelativeRotation();
		LOG_S(Warning , TEXT("Rotate Yaw %f") , jetRot.Yaw / ValueOfDivRot);
		JetRoot->AddRelativeRotation(FRotator(0 , jetRot.Yaw / ValueOfDivRot , 0));

		if (IsLeftRoll)
			JetRoot->AddRelativeRotation(-1 * RotateValue);
		if (IsRightRoll)
			JetRoot->AddRelativeRotation(RotateValue);
	}

	JetArrow->SetRelativeRotation(FRotator(0 , 0 , 0));
#pragma endregion

#pragma region 고도계
	FString CurrentMapName = UGameplayStatics::GetCurrentLevelName(GetWorld());
	if (CurrentMapName == TEXT("LHJ_TestLevel"))
	{
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
	}
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
#pragma endregion