#include "LHJ/L_Viper.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "JBS/J_Utility.h"
#include "MTVS_AirJet_Final.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Camera/CameraComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/AudioComponent.h"
#include "Components/BoxComponent.h"
#include "Components/PostProcessComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "JBS/J_MissionActorInterface.h"
#include "JBS/J_MissionPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "LHJ/L_Flare.h"
#include "LHJ/L_HUDWidget.h"
#include "LHJ/L_Missile.h"
#include "LHJ/L_RoadTrigger.h"
#include "LHJ/L_WaitingForStart.h"
#include "Net/UnrealNetwork.h"
#include "KHS/K_GameInstance.h"
#include "KHS/K_GameState.h"
#include "KHS/K_StandbyWidget.h"
#include "GameFramework/PlayerState.h"
#include "LHJ/L_Target.h"

FKey lMouse = EKeys::LeftMouseButton;
FKey rMouse = EKeys::RightMouseButton;

#pragma region Construct
AL_Viper::AL_Viper()
{
	PrimaryActorTick.bCanEverTick = true;

#pragma region Components
	JetRoot = CreateDefaultSubobject<UBoxComponent>(TEXT("JetRoot"));
	RootComponent = JetRoot;
	JetRoot->SetRelativeScale3D(FVector(10.f , 1.f , 2.f));
	JetRoot->SetSimulatePhysics(true);
	JetRoot->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	JetRoot->OnComponentBeginOverlap.AddDynamic(this , &AL_Viper::OnMyMeshOverlap);

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
	JetSprintArm->CameraRotationLagSpeed = 20.f;

	JetCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("JetCamera"));
	JetCamera->SetupAttachment(JetSprintArm);

	JetArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("JetArrow"));
	JetArrow->SetupAttachment(JetMesh);
	JetArrow->SetRelativeLocation(FVector(-1000 , 0 , 0));
	//JetArrow->SetHiddenInGame(false); // For Test

	JetFirstEngine = CreateDefaultSubobject<UBoxComponent>(TEXT("JetFirstEngine"));
	JetFirstEngine->SetupAttachment(JetMesh);
	JetFirstEngine->SetRelativeScale3D(FVector(1.5 , 1.5 , 1));
	JetFirstEngine->SetRelativeLocation(FVector(515 , -35 , 255));
	JetFirstEngine->SetBoxExtent(FVector(3 , 3 , 5));
	JetFirstEngine->SetGenerateOverlapEvents(true);
	JetFirstEngine->OnClicked.AddDynamic(this , &AL_Viper::OnMyFirstEngineClicked);
	//JetFirstEngine->SetHiddenInGame(false); // For Test

	JetMic = CreateDefaultSubobject<UBoxComponent>(TEXT("JetMic"));
	JetMic->SetupAttachment(JetMesh);
	JetMic->SetRelativeScale3D(FVector(4));
	JetMic->SetRelativeLocation(FVector(503.5 , -45 , 253));
	JetMic->SetBoxExtent(FVector(.2 , .15 , .8));
	JetMic->SetGenerateOverlapEvents(true);
	JetMic->OnClicked.AddDynamic(this , &AL_Viper::OnMyMicClicked);
	//JetMic->SetHiddenInGame(false); // For Test

	JetEngineGen = CreateDefaultSubobject<UBoxComponent>(TEXT("JetEngineGen"));
	JetEngineGen->SetupAttachment(JetMesh);
	JetEngineGen->SetRelativeScale3D(FVector(4));
	JetEngineGen->SetRelativeLocation(FVector(524 , 33 , 253));
	JetEngineGen->SetBoxExtent(FVector(.2 , .15 , .8));
	JetEngineGen->SetGenerateOverlapEvents(true);
	JetEngineGen->OnClicked.AddDynamic(this , &AL_Viper::OnMyEngineGen1Clicked);
	//JetEngineGen->SetHiddenInGame(false); // For Test

	JetEngineGen2 = CreateDefaultSubobject<UBoxComponent>(TEXT("JetEngineGen2"));
	JetEngineGen2->SetupAttachment(JetMesh);
	JetEngineGen2->SetRelativeScale3D(FVector(4));
	JetEngineGen2->SetRelativeLocation(FVector(524 , 34 , 253));
	JetEngineGen2->SetBoxExtent(FVector(.2 , .15 , .8));
	JetEngineGen2->SetGenerateOverlapEvents(true);
	JetEngineGen2->OnClicked.AddDynamic(this , &AL_Viper::OnMyEngineGen2Clicked);
	//JetEngineGen2->SetHiddenInGame(false); // For Test

	JetEngineControl = CreateDefaultSubobject<UBoxComponent>(TEXT("JetEngineControl"));
	JetEngineControl->SetupAttachment(JetMesh);
	JetEngineControl->SetRelativeScale3D(FVector(4));
	JetEngineControl->SetRelativeLocation(FVector(524 , 37.5 , 253));
	JetEngineControl->SetBoxExtent(FVector(.2 , .15 , .8));
	JetEngineControl->SetGenerateOverlapEvents(true);
	JetEngineControl->OnClicked.AddDynamic(this , &AL_Viper::OnMyEngineControlClicked);
	//JetEngineControl->SetHiddenInGame(false); // For Test

	JetEngineControl2 = CreateDefaultSubobject<UBoxComponent>(TEXT("JetEngineControl2"));
	JetEngineControl2->SetupAttachment(JetMesh);
	JetEngineControl2->SetRelativeLocation(FVector(524 , 38.5 , 253));
	JetEngineControl2->SetRelativeScale3D(FVector(4));
	JetEngineControl2->SetBoxExtent(FVector(.2 , .15 , .8));
	JetEngineControl2->SetGenerateOverlapEvents(true);
	JetEngineControl2->OnClicked.AddDynamic(this , &AL_Viper::OnMyEngineControl2Clicked);
	//JetEngineControl2->SetHiddenInGame(false); // For Test

	JetFuelStarter = CreateDefaultSubobject<UBoxComponent>(TEXT("JetFuelStarter"));
	JetFuelStarter->SetupAttachment(JetMesh);
	JetFuelStarter->SetRelativeLocation(FVector(518 , 36.5 , 253));
	JetFuelStarter->SetRelativeScale3D(FVector(4));
	JetFuelStarter->SetBoxExtent(FVector(.2 , .15 , .8));
	JetFuelStarter->SetGenerateOverlapEvents(true);
	JetFuelStarter->OnClicked.AddDynamic(this , &AL_Viper::OnMyJetFuelStarterClicked);
	//JetFuelStarter->SetHiddenInGame(false); // For Test

	JetEngineMaster = CreateDefaultSubobject<UBoxComponent>(TEXT("JetEngineMaster"));
	JetEngineMaster->SetupAttachment(JetMesh);
	JetEngineMaster->SetRelativeLocation(FVector(518 , 33.5 , 253));
	JetEngineMaster->SetRelativeScale3D(FVector(4));
	JetEngineMaster->SetBoxExtent(FVector(.2 , .15 , .8));
	JetEngineMaster->SetGenerateOverlapEvents(true);
	JetEngineMaster->OnClicked.AddDynamic(this , &AL_Viper::OnMyEngineMaster1Clicked);
	//JetEngineMaster->SetHiddenInGame(false); // For Test

	JetEngineMaster2 = CreateDefaultSubobject<UBoxComponent>(TEXT("JetEngineMaster2"));
	JetEngineMaster2->SetupAttachment(JetMesh);
	JetEngineMaster2->SetRelativeLocation(FVector(518 , 38 , 253));
	JetEngineMaster2->SetRelativeScale3D(FVector(4));
	JetEngineMaster2->SetBoxExtent(FVector(.2 , .15 , .8));
	JetEngineMaster2->SetGenerateOverlapEvents(true);
	JetEngineMaster2->OnClicked.AddDynamic(this , &AL_Viper::OnMyEngineMaster2Clicked);
	//JetEngineMaster2->SetHiddenInGame(false); // For Test

	JetJFSHandle = CreateDefaultSubobject<UBoxComponent>(TEXT("JetJFSHandle"));
	JetJFSHandle->SetupAttachment(JetMesh);
	JetJFSHandle->SetRelativeLocation(FVector(550 , 34.5 , 258));
	JetJFSHandle->SetRelativeScale3D(FVector(1));
	JetJFSHandle->SetBoxExtent(FVector(3 , 2 , 2));
	JetJFSHandle->SetGenerateOverlapEvents(true);
	JetJFSHandle->OnClicked.AddDynamic(this , &AL_Viper::OnMyJFSHandle1Clicked);
	//JetJFSHandle->SetHiddenInGame(false); // For Test

	JetCanopy = CreateDefaultSubobject<UBoxComponent>(TEXT("JetCanopy"));
	JetCanopy->SetupAttachment(JetMesh);
	JetCanopy->SetRelativeLocation(FVector(497 , 37 , 274));
	JetCanopy->SetRelativeScale3D(FVector(1.5));
	JetCanopy->SetBoxExtent(FVector(3 , 6 , 3));
	JetCanopy->SetRelativeLocation(CanopyNormalLoc);
	JetCanopy->SetGenerateOverlapEvents(true);
	JetCanopy->OnClicked.AddDynamic(this , &AL_Viper::OnMyCanopyClicked);
	//JetCanopy->SetHiddenInGame(false); // For Test

	JetBreakHold = CreateDefaultSubobject<UBoxComponent>(TEXT("JetBreakHold"));
	JetBreakHold->SetupAttachment(JetMesh);
	JetBreakHold->SetRelativeLocationAndRotation(FVector(548 , 38.5 , 259.5) , FRotator(90 , 0 , 0));
	JetBreakHold->SetRelativeScale3D(FVector(4));
	JetBreakHold->SetBoxExtent(FVector(.2 , .15 , .8));
	JetBreakHold->SetGenerateOverlapEvents(true);
	JetBreakHold->OnClicked.AddDynamic(this , &AL_Viper::OnMyBreakHoldClicked);

	JetLeftPannel = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("JetLeftPannel"));
	JetLeftPannel->SetRelativeLocation(FVector(-271 , -2 , 0));
	JetLeftPannel->SetRelativeScale3D(FVector(1.5 , 1 , 1));
	JetLeftPannel->SetupAttachment(JetMesh);

	JetRightPannel = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("JetRightPannel"));
	JetRightPannel->SetRelativeLocation(FVector(-555 , 1 , 0));
	JetRightPannel->SetRelativeScale3D(FVector(2 , 1 , 1));
	JetRightPannel->SetupAttachment(JetMesh);

	JetJFSPannel = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("JetJFSPannel"));
	JetJFSPannel->SetRelativeLocation(FVector(-137 , -6 , -17));
	JetJFSPannel->SetRelativeScale3D(FVector(1));
	JetJFSPannel->SetupAttachment(JetMesh);

	JetWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("JetWidget"));
	JetWidget->SetupAttachment(JetMesh);
	JetWidget->SetRelativeLocationAndRotation(FVector(420 , 0 , 295) , FRotator(0 , -180 , 0));
	JetWidget->SetDrawSize(FVector2D(200 , 150));

	movement = CreateDefaultSubobject<UCharacterMovementComponent>(TEXT("movement"));
	//============================================
	JetSprintArmFPS = CreateDefaultSubobject<USpringArmComponent>(TEXT("JetSprintArmFPS"));
	JetSprintArmFPS->SetupAttachment(JetMesh);
	// 조종석 뷰
	JetSprintArmFPS->SetRelativeLocationAndRotation(FVector(500 , 0 , 330) , FRotator(0 , 0 , 0));
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

	JetTailVFXLeft = CreateDefaultSubobject<UNiagaraComponent>(TEXT("JetTailVFXLeft"));
	JetTailVFXLeft->SetupAttachment(JetMesh);
	JetTailVFXLeft->SetRelativeLocationAndRotation(FVector(-390 , -580 , 180) , FRotator(0 , 180 , 0));
	ConstructorHelpers::FObjectFinder<UNiagaraSystem> TailLeftVFX(TEXT(
		"/Script/Niagara.NiagaraSystem'/Game/Asset/ArmyVFX/Niagara/Jet/NS_Jet_Trails.NS_Jet_Trails'"));
	if (TailLeftVFX.Succeeded())
	{
		JetTailVFXLeft->SetAsset(TailLeftVFX.Object);
		JetTailVFXLeft->SetFloatParameter(FName("Lifetime") , 0.f);
	}

	JetTailVFXRight = CreateDefaultSubobject<UNiagaraComponent>(TEXT("JetTailVFXRight"));
	JetTailVFXRight->SetupAttachment(JetMesh);
	JetTailVFXRight->SetRelativeLocationAndRotation(FVector(-390 , 580 , 180) , FRotator(0 , 180 , 0));
	ConstructorHelpers::FObjectFinder<UNiagaraSystem> TailRightVFX(TEXT(
		"/Script/Niagara.NiagaraSystem'/Game/Asset/ArmyVFX/Niagara/Jet/NS_Jet_Trails.NS_Jet_Trails'"));
	if (TailRightVFX.Succeeded())
	{
		JetTailVFXRight->SetAsset(TailRightVFX.Object);
		JetTailVFXRight->SetFloatParameter(FName("Lifetime") , 0.f);
	}

	JetAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("JetAudio"));
	JetAudio->SetupAttachment(RootComponent);

	//============================================
	MissileMoveLoc = CreateDefaultSubobject<USceneComponent>(TEXT("MissileMoveLoc"));
	MissileMoveLoc->SetupAttachment(RootComponent);
	MissileMoveLoc->SetRelativeLocation(FVector(0 , 0 , -200));

	FlareMoveLoc1 = CreateDefaultSubobject<USceneComponent>(TEXT("FlareMoveLoc1"));
	FlareMoveLoc1->SetupAttachment(RootComponent);
	FlareMoveLoc1->SetRelativeLocation(FVector(600 , 0 , -200));

	FlareMoveLoc2 = CreateDefaultSubobject<USceneComponent>(TEXT("FlareMoveLoc2"));
	FlareMoveLoc2->SetupAttachment(RootComponent);
	FlareMoveLoc2->SetRelativeLocation(FVector(500 , 0 , -600));

	FlareMoveLoc3 = CreateDefaultSubobject<USceneComponent>(TEXT("FlareMoveLoc3"));
	FlareMoveLoc3->SetupAttachment(RootComponent);
	FlareMoveLoc3->SetRelativeLocation(FVector(400 , 0 , -600));
	//============================================
	JetFlareArrow3 = CreateDefaultSubobject<UArrowComponent>(TEXT("JetFlareArrow3"));
	JetFlareArrow3->SetupAttachment(JetMesh);
	JetFlareArrow3->SetRelativeLocationAndRotation(FVector(-500 , 100 , 0) , FRotator(-120 , 0 , 0));
	//JetFlareArrow1->SetHiddenInGame(false); // For Test
	JetFlareArrow2 = CreateDefaultSubobject<UArrowComponent>(TEXT("JetFlareArrow2"));
	JetFlareArrow2->SetupAttachment(JetMesh);
	JetFlareArrow2->SetRelativeLocationAndRotation(FVector(-500 , -100 , 0) , FRotator(-120 , 0 , 0));
	//JetFlareArrow2->SetHiddenInGame(false); // For Test

	JetPostProcess = CreateDefaultSubobject<UPostProcessComponent>(TEXT("JetPostProcess"));
#pragma endregion

	CreateDumyComp();

	PushQueue();

	this->Tags = TArray<FName>();
	this->Tags.Add(FName("Viper"));

	// 초기 회전값 설정
	QuatCurrentRotation = FQuat::Identity;
	QuatTargetRotation = FQuat::Identity;

	bReplicates = true;
	SetReplicateMovement(true);
}
#pragma endregion

void AL_Viper::PushQueue()
{
	StartScenario.push("MIC");
	StartScenario.push("EngineGen");
	StartScenario.push("EngineControl");
	StartScenario.push("EngineMaster");
	StartScenario.push("JFS_Switch");
	StartScenario.push("JFS_Handle");
	StartScenario.push("Throttle");
	StartScenario.push("Canopy");
	StartScenario.push("BreakHold");
}

void AL_Viper::OnMyMeshOverlap(UPrimitiveComponent* OverlappedComponent , AActor* OtherActor ,
                               UPrimitiveComponent* OtherComp , int32 OtherBodyIndex , bool bFromSweep ,
                               const FHitResult& SweepResult)
{
	//LOG_SCREEN("%s" , *OtherActor->GetName());
	if (auto RT = Cast<AL_RoadTrigger>(OtherActor))
	{
		if (RT->TriggerIdx == 0)
		{
			intTriggerNum = 1;
		}
		else if (RT->TriggerIdx == 1)
		{
			intTriggerNum = 2;
			IsFlyStart = true;
		}
	}
}

void AL_Viper::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AL_Viper , CurrentWeapon);
	DOREPLIFETIME(AL_Viper , FlareCurCnt);
	DOREPLIFETIME(AL_Viper , CanopyPitch);
	DOREPLIFETIME(AL_Viper , FrontWheel);
	DOREPLIFETIME(AL_Viper , RearWheel);
	//DOREPLIFETIME(AL_Viper , ReadyMemeberCnt);
}

#pragma region Input
// Called to bind functionality to input
void AL_Viper::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
#pragma region Enhanced Input
	UEnhancedInputComponent* input = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	if (input)
	{
#pragma region Keyboard & Moused
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

		input->BindAction(IA_ViperRotateViewTrigger , ETriggerEvent::Started , this ,
		                  &AL_Viper::F_ViperRotateTriggerStarted);
		input->BindAction(IA_ViperRotateViewTrigger , ETriggerEvent::Completed , this ,
		                  &AL_Viper::F_ViperRotateTriggerCompleted);

		input->BindAction(IA_ViperZoonIn , ETriggerEvent::Started , this ,
		                  &AL_Viper::F_ViperZoomInStarted);
		input->BindAction(IA_ViperZoonIn , ETriggerEvent::Completed , this ,
		                  &AL_Viper::F_ViperZoomInCompleted);

		input->BindAction(IA_ViperZoonOut , ETriggerEvent::Started , this ,
		                  &AL_Viper::F_ViperZoomOutStarted);
		input->BindAction(IA_ViperZoonOut , ETriggerEvent::Completed , this ,
		                  &AL_Viper::F_ViperZoomOutCompleted);

		input->BindAction(IA_ViperVoice , ETriggerEvent::Started , this ,
		                  &AL_Viper::F_ViperVoiceStarted);

		input->BindAction(IA_ViperDevelop , ETriggerEvent::Started , this ,
		                  &AL_Viper::F_ViperDevelopStarted);

		input->BindAction(IA_ViperMove , ETriggerEvent::Triggered , this ,
		                  &AL_Viper::F_ViperMoveTrigger);
		input->BindAction(IA_ViperMove , ETriggerEvent::Completed , this ,
		                  &AL_Viper::F_ViperMoveCompleted);
#pragma endregion

#pragma region Controller
		input->BindAction(IA_ThrottleButton8 , ETriggerEvent::Started , this , &AL_Viper::F_ThrottleButton8Started);
		input->BindAction(IA_ThrottleButton15 , ETriggerEvent::Started , this , &AL_Viper::F_ThrottleButton15Started);
		input->BindAction(IA_ThrottleButton34 , ETriggerEvent::Started , this , &AL_Viper::F_ThrottleButton34Started);
		input->BindAction(IA_ThrottleButton35 , ETriggerEvent::Started , this , &AL_Viper::F_ThrottleButton35Started);
		input->BindAction(IA_ThrottleButton36 , ETriggerEvent::Started , this , &AL_Viper::F_ThrottleButton36Started);
		input->BindAction(IA_ThrottleButton37 , ETriggerEvent::Started , this , &AL_Viper::F_ThrottleButton37Started);
		input->BindAction(IA_ThrottleButton38 , ETriggerEvent::Started , this , &AL_Viper::F_ThrottleButton38Started);
		input->BindAction(IA_ThrottleButton39 , ETriggerEvent::Started , this , &AL_Viper::F_ThrottleButton39Started);
		input->BindAction(IA_ThrottleButton40 , ETriggerEvent::Started , this , &AL_Viper::F_ThrottleButton40Started);
		input->BindAction(IA_ThrottleButton41 , ETriggerEvent::Started , this , &AL_Viper::F_ThrottleButton41Started);
		input->BindAction(IA_ThrottleButton42 , ETriggerEvent::Started , this , &AL_Viper::F_ThrottleButton42Started);
		input->BindAction(IA_ThrottleButton43 , ETriggerEvent::Started , this , &AL_Viper::F_ThrottleButton43Started);
		input->BindAction(IA_ThrottleButton46 , ETriggerEvent::Started , this , &AL_Viper::F_ThrottleButton46Started);
		input->BindAction(IA_ThrottleButton47 , ETriggerEvent::Started , this , &AL_Viper::F_ThrottleButton47Started);
		input->BindAction(IA_ThrottleAxis4 , ETriggerEvent::Triggered , this , &AL_Viper::F_ThrottleAxis4);
		input->BindAction(IA_ThrottleAxis6 , ETriggerEvent::Triggered , this , &AL_Viper::F_ThrottleAxis6);
		input->BindAction(IA_StickButton1 , ETriggerEvent::Started , this , &AL_Viper::F_StickButton1Started);
		input->BindAction(IA_StickButton2 , ETriggerEvent::Started , this , &AL_Viper::F_StickButton2Started);
		input->BindAction(IA_StickButton5 , ETriggerEvent::Started , this , &AL_Viper::F_StickButton5Started);
		input->BindAction(IA_StickButton11 , ETriggerEvent::Started , this , &AL_Viper::F_StickButton11Started);
		input->BindAction(IA_StickButton11 , ETriggerEvent::Completed , this , &AL_Viper::F_StickButton11Completed);
		input->BindAction(IA_StickButton13 , ETriggerEvent::Started , this , &AL_Viper::F_StickButton13Started);
		input->BindAction(IA_StickButton13 , ETriggerEvent::Completed , this , &AL_Viper::F_StickButton13Completed);
		input->BindAction(IA_StickAxis1 , ETriggerEvent::Triggered , this , &AL_Viper::F_StickAxis1);
		input->BindAction(IA_StickAxis2 , ETriggerEvent::Triggered , this , &AL_Viper::F_StickAxis2);
		input->BindAction(IA_StickAxis3 , ETriggerEvent::Triggered , this , &AL_Viper::F_StickAxis3);
#pragma endregion
	}
#pragma endregion
}

void AL_Viper::OnMyFirstEngineClicked(UPrimitiveComponent* TouchedComponent , FKey ButtonPressed)
{
	if (!bFirstEngine)
	{
		auto SizeValue = ThrottleMaxLoc.X - ThrottleOffLoc.X;
		auto per = SizeValue * 25 / 100;
		JetFirstEngine->SetRelativeLocation(FVector(ThrottleOffLoc.X + per , JetFirstEngine->GetRelativeLocation().Y ,
		                                            JetFirstEngine->GetRelativeLocation().Z));
		AccelGear = 1;
		bFirstEngine = true;
	}
}

void AL_Viper::OnMyMicClicked(UPrimitiveComponent* TouchedComponent , FKey ButtonPressed)
{
	//LOG_SCREEN("MIC 클릭");
	CRPC_PlaySwitchSound(TouchedComponent->GetComponentLocation());
	if (!bMIC)
	{
		bMIC = true;
		DummyMICMesh->SetRelativeRotation(FRotator(-30 , 0 , 0));
	}
	else
	{
		bMIC = false;
		DummyMICMesh->SetRelativeRotation(FRotator(30 , 0 , 0));
	}
}

void AL_Viper::OnMyEngineGen1Clicked(UPrimitiveComponent* TouchedComponent , FKey ButtonPressed)
{
	//LOG_SCREEN("EngineGen 클릭");
	CRPC_PlaySwitchSound(TouchedComponent->GetComponentLocation());
	if (!bEngineGen1)
	{
		bEngineGen1 = true;
		DummyEngineGenerMesh1->SetRelativeRotation(FRotator(-30 , 0 , 0));
	}
	else
	{
		bEngineGen1 = false;
		DummyEngineGenerMesh1->SetRelativeRotation(FRotator(30 , 0 , 0));
	}
}

void AL_Viper::OnMyEngineGen2Clicked(UPrimitiveComponent* TouchedComponent , FKey ButtonPressed)
{
	//LOG_SCREEN("EngineGen2 클릭");
	CRPC_PlaySwitchSound(TouchedComponent->GetComponentLocation());
	if (!bEngineGen2)
	{
		bEngineGen2 = true;
		DummyEngineGenerMesh2->SetRelativeRotation(FRotator(-30 , 0 , 0));
	}
	else
	{
		bEngineGen2 = false;
		DummyEngineGenerMesh2->SetRelativeRotation(FRotator(30 , 0 , 0));
	}
}

void AL_Viper::OnMyEngineControlClicked(UPrimitiveComponent* TouchedComponent , FKey ButtonPressed)
{
	//LOG_SCREEN("EngineControl 클릭");
	CRPC_PlaySwitchSound(TouchedComponent->GetComponentLocation());
	if (!bEngineControl1)
	{
		bEngineControl1 = true;
		DummyEngineControlMesh1->SetRelativeRotation(FRotator(-30 , 0 , 0));
	}
	else
	{
		bEngineControl1 = false;
		DummyEngineControlMesh1->SetRelativeRotation(FRotator(30 , 0 , 0));
	}
}

void AL_Viper::OnMyEngineControl2Clicked(UPrimitiveComponent* TouchedComponent , FKey ButtonPressed)
{
	//LOG_SCREEN("EngineControl2 클릭");
	CRPC_PlaySwitchSound(TouchedComponent->GetComponentLocation());
	if (!bEngineControl2)
	{
		bEngineControl2 = true;
		DummyEngineControlMesh2->SetRelativeRotation(FRotator(-30 , 0 , 0));
	}
	else
	{
		bEngineControl2 = false;
		DummyEngineControlMesh2->SetRelativeRotation(FRotator(30 , 0 , 0));
	}
}

void AL_Viper::OnMyJetFuelStarterClicked(UPrimitiveComponent* TouchedComponent , FKey ButtonPressed)
{
	//LOG_SCREEN("JFS 클릭");
	CRPC_PlaySwitchSound(TouchedComponent->GetComponentLocation());
	if (!bJFS)
	{
		bJFS = true;
		DummyJFSMesh->SetRelativeRotation(FRotator(-30 , 0 , 0));
	}
	else
	{
		bJFS = false;
		DummyJFSMesh->SetRelativeRotation(FRotator(30 , 0 , 0));
	}
}

void AL_Viper::OnMyEngineMaster1Clicked(UPrimitiveComponent* TouchedComponent , FKey ButtonPressed)
{
	//LOG_SCREEN("Engine Master1 클릭");
	CRPC_PlaySwitchSound(TouchedComponent->GetComponentLocation());
	if (!bEngineMaster1)
	{
		bEngineMaster1 = true;
		DummyEngineMasterMesh1->SetRelativeRotation(FRotator(-30 , 0 , 0));
	}
	else
	{
		bEngineMaster1 = false;
		DummyEngineMasterMesh1->SetRelativeRotation(FRotator(30 , 0 , 0));
	}
}

void AL_Viper::OnMyEngineMaster2Clicked(UPrimitiveComponent* TouchedComponent , FKey ButtonPressed)
{
	//LOG_SCREEN("Engine Master2 클릭");
	CRPC_PlaySwitchSound(TouchedComponent->GetComponentLocation());
	if (!bEngineMaster2)
	{
		bEngineMaster2 = true;
		DummyEngineMasterMesh2->SetRelativeRotation(FRotator(-30 , 0 , 0));
	}
	else
	{
		bEngineMaster2 = false;
		DummyEngineMasterMesh2->SetRelativeRotation(FRotator(30 , 0 , 0));
	}
}

void AL_Viper::OnMyJFSHandle1Clicked(UPrimitiveComponent* TouchedComponent , struct FKey ButtonPressed)
{
	//LOG_SCREEN("JFS 핸들 클릭");
	CRPC_PlaySwitchSound(TouchedComponent->GetComponentLocation());
	if (!bJFSHandle)
	{
		bJFSHandle = true;
		DummyJFSHandleMesh->AddRelativeLocation(FVector(-1 , 0 , 0));
		FTimerHandle timerHandle;
		GetWorld()->GetTimerManager().SetTimer(timerHandle , [&]()
		{
			DummyJFSHandleMesh->AddRelativeLocation(FVector(1 , 0 , 0));
		} , 1.f , false);
	}
	else
	{
		bJFSHandle = false;
	}
}

void AL_Viper::OnMyCanopyClicked(UPrimitiveComponent* TouchedComponent , struct FKey ButtonPressed)
{
	CRPC_PlaySwitchSound(TouchedComponent->GetComponentLocation());
	auto currLoc = JetCanopy->GetRelativeLocation();

	if (FVector::Dist(currLoc , CanopyCloseLoc) <= 1)
	{
		JetCanopy->SetRelativeLocation(CanopyHoldLoc);
		iCanopyNum = 3;
	}
	else if (FVector::Dist(currLoc , CanopyNormalLoc) <= 1)
	{
		JetCanopy->SetRelativeLocation(CanopyCloseLoc);
		iCanopyNum = 2;
	}
	else if (FVector::Dist(currLoc , CanopyOpenLoc) <= 1)
	{
		JetCanopy->SetRelativeLocation(CanopyNormalLoc);
		iCanopyNum = 1;
	}
}

void AL_Viper::OnMyBreakHoldClicked(UPrimitiveComponent* TouchedComponent , struct FKey ButtonPressed)
{
	CRPC_PlaySwitchSound(TouchedComponent->GetComponentLocation());
	if (!bBreakHold)
	{
		bBreakHold = true;
		DummyJFSBreakHold->SetRelativeRotation(FRotator(-30 , 0 , 0));
	}
	else
	{
		bBreakHold = false;
		DummyJFSBreakHold->SetRelativeRotation(FRotator(30 , 0 , 0));
	}
}

void AL_Viper::F_ViperEngine(const FInputActionValue& value)
{
	// bool b = value.Get<bool>();
	// IsEngineOn = !IsEngineOn;
	// LOG_SCREEN("%s" , IsEngineOn?TEXT("True"):TEXT("false"));
}

void AL_Viper::F_ViperLook(const FInputActionValue& value)
{
	auto v = value.Get<FVector2D>();
	// AddControllerYawInput(v.X);
	// AddControllerPitchInput(v.Y);
	if (IsRotateTrigger)
	{
		if (JetCamera->IsActive())
		{
			FRotator TPSrot = JetSprintArm->GetRelativeRotation();
			float newTpsYaw = TPSrot.Yaw + v.X;
			float newTpsPitch = TPSrot.Pitch + v.Y;
			newTpsYaw = UKismetMathLibrary::FClamp(newTpsYaw , -360.f , 360.f);
			newTpsPitch = UKismetMathLibrary::FClamp(newTpsPitch , -80.f , 80.f);
			JetSprintArm->SetRelativeRotation(FRotator(newTpsPitch , newTpsYaw , 0));
		}
		else
		{
			FRotator FPSrot = JetSprintArmFPS->GetRelativeRotation();
			float newFpsYaw = FPSrot.Yaw + v.X;
			float newFpsPitch = FPSrot.Pitch + v.Y;
			newFpsYaw = UKismetMathLibrary::FClamp(newFpsYaw , -100.f , 100.f);
			newFpsPitch = UKismetMathLibrary::FClamp(newFpsPitch , -100.f , 100.f);
			JetSprintArmFPS->SetRelativeRotation(FRotator(newFpsPitch , newFpsYaw , 0));
		}
	}
}

void AL_Viper::F_ViperZoomInStarted(const struct FInputActionValue& value)
{
	// 중복 키입력 방지용
	if (!IsZoomOut)
		IsZoomIn = true;
}

void AL_Viper::F_ViperZoomInCompleted(const struct FInputActionValue& value)
{
	IsZoomIn = false;
}

void AL_Viper::F_ViperZoomOutStarted(const struct FInputActionValue& value)
{
	// 중복 키입력 방지용
	if (!IsZoomIn)
		IsZoomOut = true;
}

void AL_Viper::F_ViperZoomOutCompleted(const struct FInputActionValue& value)
{
	IsZoomOut = false;
}

void AL_Viper::F_ViperUpTrigger(const FInputActionValue& value)
{
	// if (!IsKeyDownPress)
	// 	IsKeyUpPress = true;

	// if (CurrentTime < ChangeTime)
	// 	return;
	//
	// CurrentTime = 0.f;
	// ForceUnitRot = CombineRotate(ChangeMoveVector);
}

void AL_Viper::F_ViperUpCompleted(const FInputActionValue& value)
{
	//IsKeyUpPress = false;
	// ForceUnitRot = FRotator(0 , 0 , 0);
}

void AL_Viper::F_ViperDownTrigger(const FInputActionValue& value)
{
	// if (!IsKeyUpPress)
	// 	IsKeyDownPress = true;

	// if (CurrentTime < ChangeTime)
	// 	return;
	//
	// CurrentTime = 0.f;
	// ForceUnitRot = CombineRotate(-1 * ChangeMoveVector);
}

void AL_Viper::F_ViperDownCompleted(const FInputActionValue& value)
{
	//IsKeyDownPress = false;
	// ForceUnitRot = FRotator(0 , 0 , 0);
}

void AL_Viper::F_ViperRightTrigger(const FInputActionValue& value)
{
	// if (!IsKeyLeftPress)
	// 	IsKeyRightPress = true;
}

void AL_Viper::F_ViperRightCompleted(const FInputActionValue& value)
{
	//IsKeyRightPress = false;
}

void AL_Viper::F_ViperLeftTrigger(const FInputActionValue& value)
{
	// if(!IsKeyRightPress)
	// 	IsKeyLeftPress = true;
}

void AL_Viper::F_ViperLeftCompleted(const FInputActionValue& value)
{
	//IsKeyLeftPress = false;
}

void AL_Viper::F_ViperTurnRightTrigger(const FInputActionValue& value)
{
	// if(!IsLeftRoll)
	// 	IsRightRoll = true;
}

void AL_Viper::F_ViperTurnRightCompleted(const FInputActionValue& value)
{
	//IsRightRoll = false;
}

void AL_Viper::F_ViperTurnLeftTrigger(const FInputActionValue& value)
{
	// if(!IsRightRoll)
	// 	IsLeftRoll = true;
}

void AL_Viper::F_ViperTurnLeftCompleted(const FInputActionValue& value)
{
	//IsLeftRoll = false;
}

void AL_Viper::F_ViperResetRotation(const FInputActionValue& value)
{
	SetActorRotation(FRotator(0 , JetMesh->GetComponentRotation().Yaw , 0));
	JetArrow->SetRelativeRotation(FRotator(0 , JetArrow->GetRelativeRotation().Yaw , 0));
}

void AL_Viper::F_ViperAccelStarted(const FInputActionValue& value)
{
	if (!bThrottleBreak)
		bThrottleAccel = true;
	// AccelGear++;
	// if (AccelGear > 3)
	// 	AccelGear = 3;
	// IsAccel = true;
}

void AL_Viper::F_ViperAccelCompleted(const FInputActionValue& value)
{
	bThrottleAccel = false;
	// KeyDownAccel = 0.f;
	// IsAccel = false;
}

void AL_Viper::F_ViperBreakStarted(const FInputActionValue& value)
{
	if (!bThrottleAccel)
		bThrottleBreak = true;
	// AccelGear--;
	// if (AccelGear < 0)
	// 	AccelGear = 0;
	// IsBreak = true;
}

void AL_Viper::F_ViperBreakCompleted(const FInputActionValue& value)
{
	bThrottleBreak = false;
	// KeyDownAccel = 0.f;
	// IsBreak = false;
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
	{
		if (JetPostProcess && JetPostProcess->Settings.WeightedBlendables.Array.Num() > 0)
			JetPostProcess->Settings.WeightedBlendables.Array[0].Weight = 1;
		JetCameraFPS->SetActive(true);
	}
}

void AL_Viper::F_ViperTpsStarted(const struct FInputActionValue& value)
{
	if (JetCamera)
		JetCamera->SetActive(true);
	if (JetCameraFPS)
	{
		if (JetPostProcess && JetPostProcess->Settings.WeightedBlendables.Array.Num() > 0)
			JetPostProcess->Settings.WeightedBlendables.Array[0].Weight = 0;
		JetCameraFPS->SetActive(false);
	}
}

void AL_Viper::F_ViperChangeWeaponStarted(const struct FInputActionValue& value)
{
	// 현재 값에서 1을 더하고, Max로 나눈 나머지를 사용하여 순환
	CurrentWeapon = static_cast<EWeapon>((static_cast<int32>(CurrentWeapon) + 1) % static_cast<int32>(EWeapon::Max));
}

void AL_Viper::F_ViperRotateTriggerStarted(const struct FInputActionValue& value)
{
	IsRotateTrigger = true;
	// Value가 True일 때만 처리
	if (value.Get<bool>())
	{
		PerformLineTrace();
	}
}

void AL_Viper::F_ViperRotateTriggerCompleted(const struct FInputActionValue& value)
{
	IsRotateTrigger = false;
}

void AL_Viper::F_ViperVoiceStarted(const struct FInputActionValue& value)
{
	bVoice = !bVoice;
	if (bVoice)
		StartVoiceChat();
	else
		StopVoiceChat();
}

void AL_Viper::F_ViperDevelopStarted(const struct FInputActionValue& value)
{
#pragma region 시동절차 자동 수행
	bReadyTimeEndFlag = true;
	if (!IsStart)
	{
		while (true)
		{
			FString ScenarioFront = StartScenario.front();
			if (ScenarioFront.Equals("MIC"))
			{
				if (JetMic && !bMIC)
				{
					OnMyMicClicked(JetMic , lMouse);
				}
				StartScenario.pop();
				DummyMICMesh->SetRenderCustomDepth(false);
				DummyMICMesh->CustomDepthStencilValue = 0;
			}
			else if (ScenarioFront.Equals("EngineGen"))
			{
				if (JetEngineGen && !bEngineGen1)
					OnMyEngineGen1Clicked(JetEngineGen , lMouse);
				if (JetEngineGen2 && !bEngineGen2)
					OnMyEngineGen2Clicked(JetEngineGen2 , lMouse);
				StartScenario.pop();
				DummyEngineGenerMesh1->SetRenderCustomDepth(false);
				DummyEngineGenerMesh1->CustomDepthStencilValue = 0;
				DummyEngineGenerMesh2->SetRenderCustomDepth(false);
				DummyEngineGenerMesh2->CustomDepthStencilValue = 0;
			}
			else if (ScenarioFront.Equals("EngineControl"))
			{
				if (JetEngineControl && !bEngineControl1)
					OnMyEngineControlClicked(JetEngineControl , lMouse);
				if (JetEngineControl2 && !bEngineControl2)
					OnMyEngineControl2Clicked(JetEngineControl2 , lMouse);
				StartScenario.pop();
				DummyEngineControlMesh1->SetRenderCustomDepth(false);
				DummyEngineControlMesh1->CustomDepthStencilValue = 0;
				DummyEngineControlMesh2->SetRenderCustomDepth(false);
				DummyEngineControlMesh2->CustomDepthStencilValue = 0;
			}
			else if (ScenarioFront.Equals("EngineMaster"))
			{
				if (JetEngineMaster && !bEngineMaster1)
					OnMyEngineMaster1Clicked(JetEngineMaster , lMouse);
				if (JetEngineMaster2 && !bEngineMaster2)
					OnMyEngineMaster2Clicked(JetEngineMaster2 , lMouse);
				StartScenario.pop();
				DummyEngineMasterMesh1->SetRenderCustomDepth(false);
				DummyEngineMasterMesh1->CustomDepthStencilValue = 0;
				DummyEngineMasterMesh2->SetRenderCustomDepth(false);
				DummyEngineMasterMesh2->CustomDepthStencilValue = 0;
			}
			else if (ScenarioFront.Equals("JFS_Switch"))
			{
				if (JetFuelStarter && !bJFS)
					OnMyJetFuelStarterClicked(JetFuelStarter , lMouse);

				StartScenario.pop();
				DummyJFSMesh->SetRenderCustomDepth(false);
				DummyJFSMesh->CustomDepthStencilValue = 0;
			}
			else if (ScenarioFront.Equals("JFS_Handle"))
			{
				if (JetJFSHandle && !bJFSHandle)
					OnMyJFSHandle1Clicked(JetJFSHandle , lMouse);
				StartScenario.pop();
				DummyJFSHandleMesh->SetRenderCustomDepth(false);
				DummyJFSHandleMesh->CustomDepthStencilValue = 0;
			}
			else if (ScenarioFront.Equals("Throttle"))
			{
				if (JetFirstEngine && !bFirstEngine)
					OnMyFirstEngineClicked(JetFirstEngine , lMouse);
				StartScenario.pop();
				DummyThrottleMesh->SetRenderCustomDepth(false);
				DummyThrottleMesh->CustomDepthStencilValue = 0;
			}
			else if (ScenarioFront.Equals("Canopy"))
			{
				if (JetCanopy && iCanopyNum != 2)
				{
					JetCanopy->SetRelativeLocation(CanopyCloseLoc);
					iCanopyNum = 2;
				}

				break;
			}
		}
	}
#pragma endregion
	//IsStart = true;
	//IsEngineOn = true;
	//intTriggerNum = 2;
	//IsFlyStart = true;
}

void AL_Viper::F_ViperMoveTrigger(const struct FInputActionValue& value)
{
	FVector2D moveVector = value.Get<FVector2D>();
	LOG_SCREEN("%f, %f" , moveVector.X , moveVector.Y);

	// 입력값에 최대 회전 각도 제한 적용
	float RollAngle = 0.f;
	float PitchAngle = 0.f;

	if ((moveVector.Y > 0.6f || moveVector.Y < -0.6f) && (moveVector.X > 0.6f || moveVector.X < -0.6f))
	{
		RollAngle = moveVector.Y * MaxRotationAngle / 3.f;
		PitchAngle = moveVector.X * MaxRotationAngle / 3.f;
	}
	else
	{
		RollAngle = moveVector.Y * MaxRotationAngle / 5.f;
		PitchAngle = moveVector.X * MaxRotationAngle / 6.f;
	}

	// Roll과 Pitch를 쿼터니언 회전으로 변환
	FQuat RollRotation = FQuat(FVector(1 , 0 , 0) , FMath::DegreesToRadians(RollAngle));
	FQuat PitchRotation = FQuat(FVector(0 , 1 , 0) , FMath::DegreesToRadians(PitchAngle));

	// 목표 회전 설정 (RootComponent를 기준으로)
	QuatTargetRotation = QuatCurrentRotation * RollRotation * PitchRotation;
	// if (moveVector == FVector2D(0 , 1))
	// {
	// 	IsLeftRoll = true;
	// 	IsKeyUpPress = false;
	// 	IsKeyDownPress = false;
	// 	IsRightRoll = false;
	// }
	// else if (moveVector == FVector2D(0 , -1))
	// {
	// 	IsRightRoll = true;
	// 	IsKeyUpPress = false;
	// 	IsKeyDownPress = false;
	// 	IsLeftRoll = false;
	// }
	// else if (moveVector == FVector2D(-1 , 0))
	// {
	// 	IsKeyUpPress = true;
	// 	IsKeyDownPress = false;
	// 	IsLeftRoll = false;
	// 	IsRightRoll = false;
	// }
	// else if (moveVector == FVector2D(1 , 0))
	// {
	// 	IsKeyDownPress = true;
	// 	IsKeyUpPress = false;
	// 	IsLeftRoll = false;
	// 	IsRightRoll = false;
	// }
	// 스틱 입력값을 각도로 변환 (최대 회전 각도 제한 적용)
	// float RollAngle = moveVector.Y * MaxRotationAngle;
	// float PitchAngle = moveVector.X * MaxRotationAngle;
	//
	// // Roll과 Pitch 회전을 위한 쿼터니언 생성
	// FQuat RollRotation = FQuat(FVector(1.0f, 0.0f, 0.0f), FMath::DegreesToRadians(RollAngle));
	// FQuat PitchRotation = FQuat(FVector(0.0f, 1.0f, 0.0f), FMath::DegreesToRadians(PitchAngle));
	//
	// // Roll과 Pitch 회전을 결합
	// QuatTargetRotation = RollRotation * PitchRotation;
}

void AL_Viper::F_ViperMoveCompleted(const struct FInputActionValue& value)
{
	QuatTargetRotation = QuatCurrentRotation;
	IsRightRoll = false;
	IsLeftRoll = false;
	IsKeyUpPress = false;
	IsKeyDownPress = false;
}

// FRotator AL_Viper::CombineRotate(FVector NewVector)
// {
// 	FRotator loc_rot = FRotator(NewVector.Y , NewVector.X , NewVector.Z);
// 	return FRotator(ForceUnitRot.Pitch + loc_rot.Pitch , ForceUnitRot.Yaw + loc_rot.Yaw ,
// 	                ForceUnitRot.Roll + loc_rot.Roll);
// }
#pragma endregion

void AL_Viper::BeginPlay()
{
	Super::BeginPlay();

	CRPC_AudioControl(false);

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
	if (JetPostProcess && JetPostProcess->Settings.WeightedBlendables.Array.Num() > 0)
		JetPostProcess->Settings.WeightedBlendables.Array[0].Weight = 0;

	FString CurrentMapName = UGameplayStatics::GetCurrentLevelName(GetWorld());
	if (CurrentMapName == FString::Printf(TEXT("CesiumTest")))
	{
		auto KGameInstace = CastChecked<UK_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
		if (!KGameInstace)
		{
			LOG_S(Warning , TEXT("GameInstance doesn't exist"));
		}
		//GI에서 자신의 로그인 ID를 받아오고
		FString MyUserID = KGameInstace->GetUserId();

		auto kpc = Cast<AK_PlayerController>(GetOwner());
		//클라이언트일때
		if (kpc && kpc->IsLocalController())
		{
			LOG_S(Warning , TEXT("MyUserID : %s") , *MyUserID);
			//ServerRPC함수를 호출
			ServerRPC_SetConnectedPlayerNames(MyUserID);
		}

		// 	FTimerHandle TimerHandle;
		// 	GetWorldTimerManager().SetTimer(TimerHandle , [this]()
		// 	{
		// 		LOG_S(Warning , TEXT("%s") , IsStart?*FString("true"):*FString("false"));
		// 		bReadyTimeEndFlag = true;
		// 		if (!IsStart)
		// 		{
		// 			FKey lMouse = EKeys::LeftMouseButton;
		// 			if (JetMic && !bMIC)
		// 				OnMyMicClicked(JetMic , lMouse);
		// 			if (JetEngineGen && !bEngineGen1)
		// 				OnMyEngineGen1Clicked(JetEngineGen , lMouse);
		// 			if (JetEngineGen2 && !bEngineGen2)
		// 				OnMyEngineGen2Clicked(JetEngineGen2 , lMouse);
		// 			if (JetEngineControl && !bEngineControl1)
		// 				OnMyEngineControlClicked(JetEngineControl , lMouse);
		// 			if (JetEngineControl2 && !bEngineControl2)
		// 				OnMyEngineControl2Clicked(JetEngineControl2 , lMouse);
		// 			if (JetEngineMaster && !bEngineMaster1)
		// 				OnMyEngineMaster1Clicked(JetEngineMaster , lMouse);
		// 			if (JetEngineMaster2 && !bEngineMaster2)
		// 				OnMyEngineMaster2Clicked(JetEngineMaster2 , lMouse);
		// 			if (JetFuelStarter && !bJFS)
		// 				OnMyJetFuelStarterClicked(JetFuelStarter , lMouse);
		// 			if (JetJFSHandle && !bJFSHandle)
		// 				OnMyJFSHandle1Clicked(JetJFSHandle , lMouse);
		// 			if (JetFirstEngine && !bFirstEngine)
		// 				OnMyFirstEngineClicked(JetFirstEngine , lMouse);
		// 			if (JetCanopy && iCanopyNum != 2)
		// 			{
		// 				JetCanopy->SetRelativeLocation(CanopyCloseLoc);
		// 				iCanopyNum = 2;
		// 			}
		// 		}
		// 	} , TimeToReady , false);
	}

	// if (auto PC = Cast<AJ_MissionPlayerController>(GetOwner()))
	// {
	// 	if (PC->WaitingForStartFac)
	// 	{
	// 		WaitingForStartUI = CreateWidget<UL_WaitingForStart>(GetWorld() , PC->WaitingForStartFac);
	// 		if (WaitingForStartUI)
	// 		{
	// 			WaitingForStartUI->AddToViewport(0);
	// 			WaitingForStartUI->SetVisibility(ESlateVisibility::Hidden);
	// 		}
	// 	}
	// }
}

void AL_Viper::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// PrintNetLog();

	// 제트엔진 이펙트
	if (bJetTailVFXOn)
	{
		if (JetTailVFXLeft && JetTailVFXLeft->GetAsset())
			JetTailVFXLeft->SetFloatParameter(FName("Lifetime") , 1.f);
		if (JetTailVFXRight && JetTailVFXRight->GetAsset())
			JetTailVFXRight->SetFloatParameter(FName("Lifetime") , 1.f);

		CRPC_AudioControl(true , 2);
		bJetTailVFXOn = false;
	}

#pragma region Canopy Open & Close
	if (iCanopyNum == 2)
	{
		// 캐노피를 닫는다.
		ServerRPC_Canopy(false);
	}
	else if (iCanopyNum == 0)
	{
		// 캐노피를 연다.
		ServerRPC_Canopy(true);
	}
#pragma endregion

#pragma region 사운드 감쇠
	if (JetAudio)
	{
		if (JetCameraFPS->IsActive())
		{
			if (CanopyPitch == 0)
			{
				if (!JetAudio->bAllowSpatialization)
					JetAudio->bAllowSpatialization = true;
			}
			else
			{
				if (JetAudio->bAllowSpatialization)
					JetAudio->bAllowSpatialization = false;
			}
		}
		else if (JetCamera->IsActive())
		{
			if (JetAudio->bAllowSpatialization)
				JetAudio->bAllowSpatialization = false;
		}
	}
#pragma endregion

#pragma region 마지막 트리거 박스를 통과하면 IMC_Fun을 연결
	if (IsFlyStart)
	{
		auto pc = Cast<APlayerController>(Controller);
		if (pc)
		{
			UEnhancedInputLocalPlayerSubsystem* subsys = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
				pc->GetLocalPlayer());
			if (subsys)
			{
				if (!subsys->HasMappingContext(IMC_Fun))
					subsys->AddMappingContext(IMC_Fun , 0);
			}

			pc->bEnableClickEvents = true;
		}

		if (FrontWheel < 1.f)
			ServerRPC_Wheel();
		else
			IsFlyStart = false;
	}
#pragma endregion

	// 시동 절차 단계
	if (!IsStart)
	{
		if (StartScenario.size() > 0)
		{
#pragma region 시동절차
			FString ScenarioFront = StartScenario.front();
			UStaticMeshComponent* ScenarioComponent = nullptr;
			if (ScenarioFront.Equals("MIC"))
			{
				if (DummyMICMesh)
				{
					DummyMICMesh->SetRenderCustomDepth(true);
					DummyMICMesh->CustomDepthStencilValue = 1;
					if (bMIC)
					{
						StartScenario.pop();
						engineProgSuccessDel.ExecuteIfBound(EEngineProgress::MIC_SWITCH_ON);
						DummyMICMesh->SetRenderCustomDepth(false);
						DummyMICMesh->CustomDepthStencilValue = 0;
					}
				}
			}
			else if (ScenarioFront.Equals("EngineGen"))
			{
				if (DummyEngineGenerMesh1 && DummyEngineGenerMesh2)
				{
					DummyEngineGenerMesh1->SetRenderCustomDepth(true);
					DummyEngineGenerMesh1->CustomDepthStencilValue = 1;
					DummyEngineGenerMesh2->SetRenderCustomDepth(true);
					DummyEngineGenerMesh2->CustomDepthStencilValue = 1;

					if (bEngineGen1 && bEngineGen2)
					{
						StartScenario.pop();
						engineProgSuccessDel.ExecuteIfBound(EEngineProgress::ENGINE_GEN_SWITCH_ON);
						DummyEngineGenerMesh1->SetRenderCustomDepth(false);
						DummyEngineGenerMesh1->CustomDepthStencilValue = 0;
						DummyEngineGenerMesh2->SetRenderCustomDepth(false);
						DummyEngineGenerMesh2->CustomDepthStencilValue = 0;
					}
				}
			}
			else if (ScenarioFront.Equals("EngineControl"))
			{
				if (DummyEngineControlMesh1 && DummyEngineControlMesh2)
				{
					DummyEngineControlMesh1->SetRenderCustomDepth(true);
					DummyEngineControlMesh1->CustomDepthStencilValue = 1;
					DummyEngineControlMesh2->SetRenderCustomDepth(true);
					DummyEngineControlMesh2->CustomDepthStencilValue = 1;

					if (bEngineControl1 && bEngineControl2)
					{
						StartScenario.pop();
						engineProgSuccessDel.ExecuteIfBound(EEngineProgress::ENGINE_CONTROL_SWITCH_ON);
						DummyEngineControlMesh1->SetRenderCustomDepth(false);
						DummyEngineControlMesh1->CustomDepthStencilValue = 0;
						DummyEngineControlMesh2->SetRenderCustomDepth(false);
						DummyEngineControlMesh2->CustomDepthStencilValue = 0;
					}
				}
			}
			else if (ScenarioFront.Equals("JFS_Switch"))
			{
				if (DummyJFSMesh)
				{
					DummyJFSMesh->SetRenderCustomDepth(true);
					DummyJFSMesh->CustomDepthStencilValue = 1;

					if (bJFS)
					{
						StartScenario.pop();
						engineProgSuccessDel.ExecuteIfBound(EEngineProgress::JFS_STARTER_SWITCH_ON);
						DummyJFSMesh->SetRenderCustomDepth(false);
						DummyJFSMesh->CustomDepthStencilValue = 0;
					}
				}
			}
			else if (ScenarioFront.Equals("EngineMaster"))
			{
				if (DummyEngineControlMesh1 && DummyEngineControlMesh2)
				{
					DummyEngineMasterMesh1->SetRenderCustomDepth(true);
					DummyEngineMasterMesh1->CustomDepthStencilValue = 1;
					DummyEngineMasterMesh2->SetRenderCustomDepth(true);
					DummyEngineMasterMesh2->CustomDepthStencilValue = 1;

					if (bEngineMaster1 && bEngineMaster2)
					{
						StartScenario.pop();
						engineProgSuccessDel.ExecuteIfBound(EEngineProgress::ENGINE_MASTER_SWITCH_ON);
						DummyEngineMasterMesh1->SetRenderCustomDepth(false);
						DummyEngineMasterMesh1->CustomDepthStencilValue = 0;
						DummyEngineMasterMesh2->SetRenderCustomDepth(false);
						DummyEngineMasterMesh2->CustomDepthStencilValue = 0;
					}
				}
			}
			else if (ScenarioFront.Equals("JFS_Handle"))
			{
				if (DummyJFSHandleMesh)
				{
					DummyJFSHandleMesh->SetRenderCustomDepth(true);
					DummyJFSHandleMesh->CustomDepthStencilValue = 1;

					if (bJFSHandle)
					{
						StartScenario.pop();
						engineProgSuccessDel.ExecuteIfBound(EEngineProgress::JFS_HANDLE_PULL);
						DummyJFSHandleMesh->SetRenderCustomDepth(false);
						DummyJFSHandleMesh->CustomDepthStencilValue = 0;
					}
				}
			}
			else if (ScenarioFront.Equals("Throttle"))
			{
				DummyThrottleMesh->SetRenderCustomDepth(true);
				DummyThrottleMesh->CustomDepthStencilValue = 1;

				if (bFirstEngine)
				{
					StartScenario.pop();
					engineProgSuccessDel.ExecuteIfBound(EEngineProgress::ENGINE_THROTTLE_IDLE);
					DummyThrottleMesh->SetRenderCustomDepth(false);
					DummyThrottleMesh->CustomDepthStencilValue = 0;
					CRPC_AudioControl(true , 0);
				}
			}
			else if (ScenarioFront.Equals("Canopy"))
			{
				// DummyCanopyMesh->SetRenderCustomDepth(true);
				// DummyCanopyMesh->CustomDepthStencilValue = 1;

				if (CanopyPitch > 0.f)
				{
					if (iCanopyNum == 2)
					{
						DummyCanopyMesh->SetRenderCustomDepth(false);
					}
					else
					{
						DummyCanopyMesh->SetRenderCustomDepth(true);
						DummyCanopyMesh->CustomDepthStencilValue = 1;
					}
				}

				if (CanopyPitch == 0.f)
				{
					DummyCanopyMesh->SetRenderCustomDepth(true);
					DummyCanopyMesh->CustomDepthStencilValue = 1;
					if (bReadyTimeEndFlag)
					{
						if (JetCanopy)
						{
							OnMyCanopyClicked(JetCanopy , lMouse);
							DummyCanopyMesh->SetRenderCustomDepth(false);
							DummyCanopyMesh->CustomDepthStencilValue = 0;
							StartScenario.pop();
						}
					}

					if (!bReadyTimeEndFlag && iCanopyNum == 3)
					{
						DummyCanopyMesh->SetRenderCustomDepth(false);
						DummyCanopyMesh->CustomDepthStencilValue = 0;
						StartScenario.pop();
						engineProgSuccessDel.ExecuteIfBound(EEngineProgress::CLOSE_CANOPY);
						// CRPC_AudioControl(true , 1);
					}
				}
			}
			else if (ScenarioFront.Equals("BreakHold"))
			{
				DummyJFSBreakHold->SetRenderCustomDepth(true);
				DummyJFSBreakHold->CustomDepthStencilValue = 1;

				if (bReadyTimeEndFlag)
				{
					OnMyBreakHoldClicked(JetBreakHold , lMouse);
					StartScenario.pop();
					DummyJFSBreakHold->SetRenderCustomDepth(false);
					DummyJFSBreakHold->CustomDepthStencilValue = 0;
					CRPC_AudioControl(true , 1);
				}

				if (!bReadyTimeEndFlag && bBreakHold)
				{
					StartScenario.pop();
					engineProgSuccessDel.ExecuteIfBound(EEngineProgress::RELEASE_SIDE_BREAK);
					DummyJFSBreakHold->SetRenderCustomDepth(false);
					DummyJFSBreakHold->CustomDepthStencilValue = 0;
					CRPC_AudioControl(true , 1);
				}
			}
#pragma endregion
		}
		else
		{
			if (JetPostProcess && JetPostProcess->Settings.WeightedBlendables.Array.Num() > 0)
				JetPostProcess->Settings.WeightedBlendables.Array[0].Weight = 0;
			IsStart = true;
			IsEngineOn = true;
			// if (auto pc = Cast<AJ_MissionPlayerController>(GetOwner()))
			// {
			// 	UEnhancedInputLocalPlayerSubsystem* subsys = ULocalPlayer::GetSubsystem<
			// 		UEnhancedInputLocalPlayerSubsystem>(
			// 		pc->GetLocalPlayer());
			// 	if (subsys)
			// 	{
			// 		FModifyContextOptions options;
			// 		subsys->RemoveMappingContext(IMC_Viper , options);
			// 	}
			//
			// 	if (WaitingForStartUI)
			// 	{
			// 		WaitingForStartUI->SetVisibility(ESlateVisibility::Visible);
			// 		if(HasAuthority())
			// 		{
			// 			MultiRPC_SetCurrentReadyMem(ReadyMemeberCnt);
			// 		}
			// 		else
			// 		{
			// 			ServerRPC_SetCurrentReadyMem();
			// 		}
			// 		// auto gi = Cast<UK_GameInstance>(GetGameInstance());
			// 		// gi->ReadyMemeberCnt++;
			// 		// gi->OnMyMemberReFresh();
			// 	}
			// }
		}
	}
	// 운행 단계
	else
	{
		CurrentTime += DeltaTime;

#pragma region Rotate JetArrow
		FRotator jetRot = JetArrow->GetRelativeRotation();
		// Check Distance Into Area
		// if (IsKeyUpPress || IsKeyDownPress)
		// {
		// 	if (jetRot.Pitch > MaxPitchValue)
		// 	{
		// 		JetArrow->SetRelativeRotation(FRotator(MaxPitchValue - 1.f , jetRot.Yaw , jetRot.Roll));
		// 		ForceUnitRot = FRotator(0 , 0 , 0);
		// 	}
		// 	else if (jetRot.Pitch < MinPitchValue)
		// 	{
		// 		JetArrow->SetRelativeRotation(FRotator(MinPitchValue + 1.f , jetRot.Yaw , jetRot.Roll));
		// 		ForceUnitRot = FRotator(0 , 0 , 0);
		// 	}
		// 	else
		// 	{
		// 		JetArrow->AddRelativeRotation(FRotator(ForceUnitRot.Pitch , 0 , 0));
		// 	}
		// }
		//
		// if (IsKeyLeftPress)
		// {
		// 	if (!IsKeyRightPress)
		// 	{
		// 		FRotator resetRot = FRotator(jetRot.Pitch , 0 , jetRot.Roll);
		// 		JetArrow->SetRelativeRotation(resetRot);
		// 		FRotator newRot = FRotator(jetRot.Pitch , MinYawValue , jetRot.Roll);
		// 		JetArrow->AddRelativeRotation(newRot);
		// 	}
		// }
		// else if (IsKeyRightPress)
		// {
		// 	if (!IsKeyLeftPress)
		// 	{
		// 		FRotator resetRot = FRotator(jetRot.Pitch , 0 , jetRot.Roll);
		// 		JetArrow->SetRelativeRotation(resetRot);
		// 		FRotator newRot = FRotator(jetRot.Pitch , MaxYawValue , jetRot.Roll);
		// 		JetArrow->AddRelativeRotation(newRot);
		// 	}
		// }

		// 방향전환중이 아니라면 방향을 가운데로 변환
		// if (!IsKeyLeftPress && !IsKeyRightPress)
		// 	JetArrow->SetRelativeRotation(FRotator(JetArrow->GetRelativeRotation().Pitch , 0 ,
		// 	                                       JetArrow->GetRelativeRotation().Roll));

		//LOG_SCREEN("현재 각도는 %f 입니다." , JetArrow->GetRelativeRotation().Pitch);
#pragma endregion

#pragma region Move Throttle
		FVector engineLoc = JetFirstEngine->GetRelativeLocation();

		if (bThrottleAccel)
		{
			if (intTriggerNum == 0)
			{
				auto SizeValue = ThrottleMaxLoc.X - ThrottleOffLoc.X;
				auto per = SizeValue * 25 / 100;
				FVector VecTrigger0 = FVector(ThrottleOffLoc.X + per , ThrottleOffLoc.Y , ThrottleOffLoc.Z);
				if (engineLoc.X < VecTrigger0.X)
				{
					auto newEngineX = engineLoc.X + ThrottleMoveSpeed1;
					newEngineX = UKismetMathLibrary::FClamp(newEngineX , ThrottleOffLoc.X , VecTrigger0.X);
					if (VecTrigger0.X - newEngineX < 0.2)
						JetFirstEngine->SetRelativeLocation(VecTrigger0);
					else
						JetFirstEngine->SetRelativeLocation(FVector(newEngineX , engineLoc.Y , engineLoc.Z));
				}
			}
			else if (intTriggerNum == 1)
			{
				auto SizeValue = ThrottleMaxLoc.X - ThrottleOffLoc.X;
				auto per = SizeValue * 80 / 100;
				FVector VecTrigger1 = FVector(ThrottleOffLoc.X + per , ThrottleOffLoc.Y , ThrottleOffLoc.Z);

				if (engineLoc.X < ThrottleMilLoc.X)
				{
					auto newEngineX = engineLoc.X + ThrottleMoveSpeed1;
					newEngineX = UKismetMathLibrary::FClamp(newEngineX , ThrottleOffLoc.X , ThrottleMilLoc.X);
					if (ThrottleMilLoc.X - newEngineX < 0.2)
						JetFirstEngine->SetRelativeLocation(ThrottleMilLoc);
					else
						JetFirstEngine->SetRelativeLocation(FVector(newEngineX , engineLoc.Y , engineLoc.Z));
				}
				if (engineLoc.X < VecTrigger1.X)
				{
					auto newEngineX = engineLoc.X + ThrottleMoveSpeed2;
					newEngineX = UKismetMathLibrary::FClamp(newEngineX , ThrottleOffLoc.X , VecTrigger1.X);
					if (VecTrigger1.X - newEngineX < 0.2)
						JetFirstEngine->SetRelativeLocation(VecTrigger1);
					else
						JetFirstEngine->SetRelativeLocation(FVector(newEngineX , engineLoc.Y , engineLoc.Z));
				}
			}
			else if (intTriggerNum == 2)
			{
				if (engineLoc.X < ThrottleMilLoc.X)
				{
					auto newEngineX = engineLoc.X + ThrottleMoveSpeed1;
					newEngineX = UKismetMathLibrary::FClamp(newEngineX , ThrottleOffLoc.X , ThrottleMilLoc.X);
					if (ThrottleMilLoc.X - newEngineX < 0.2)
						JetFirstEngine->SetRelativeLocation(ThrottleMilLoc);
					else
						JetFirstEngine->SetRelativeLocation(FVector(newEngineX , engineLoc.Y , engineLoc.Z));
				}
				else if (engineLoc.X < ThrottleMaxLoc.X)
				{
					auto newEngineX = engineLoc.X + ThrottleMoveSpeed2;
					newEngineX = UKismetMathLibrary::FClamp(newEngineX , ThrottleMilLoc.X , ThrottleMaxLoc.X);
					JetFirstEngine->SetRelativeLocation(FVector(newEngineX , engineLoc.Y , engineLoc.Z));
				}
			}
		}

		if (bThrottleBreak)
		{
			float newEngineX = 0.f;
			if (engineLoc.X > ThrottleMilLoc.X)
				newEngineX = engineLoc.X - ThrottleMoveSpeed2;
			else if (engineLoc.X > ThrottleOffLoc.X)
				newEngineX = engineLoc.X - ThrottleMoveSpeed1;
			newEngineX = UKismetMathLibrary::FClamp(newEngineX , ThrottleOffLoc.X , ThrottleMaxLoc.X);
			JetFirstEngine->SetRelativeLocation(FVector(newEngineX , engineLoc.Y , engineLoc.Z));
		}

		SetAccelGear();
		// LOG_S(Warning , TEXT("Current Gear X LOC : %f") , JetFirstEngine->GetRelativeLocation().X);
		// LOG_S(Warning , TEXT("Current Gear : %d") , AccelGear);
#pragma endregion

		// 현재 회전을 목표 회전으로 보간 (DeltaTime과 RotationSpeed를 사용하여 부드럽게)
		QuatCurrentRotation = FQuat::Slerp(QuatCurrentRotation , QuatTargetRotation , RotationSpeed * DeltaTime);

		// RootComponent의 회전 설정
		//RootComponent->SetWorldRotation(QuatCurrentRotation.Rotator());
		SetActorRotation(QuatCurrentRotation.Rotator());
		// #pragma region Quat Move
		// 		if (JetMesh)
		// 		{
		// 			// 현재 회전값을 목표 회전값으로 부드럽게 보간
		// 			QuatCurrentRotation = FQuat::Slerp(
		// 				QuatCurrentRotation ,
		// 				QuatTargetRotation ,
		// 				FMath::Clamp(DeltaTime * RotationSpeed / 90.0f , 0.0f , 1.0f)
		// 			);
		//
		// 			// 쿼터니언 회전 적용
		// 			JetMesh->SetRelativeRotation(QuatCurrentRotation);
		//
		// 			// 화살표 컴포넌트에도 동일한 회전 적용
		// 			if (JetArrow)
		// 			{
		// 				JetArrow->SetRelativeRotation(QuatCurrentRotation);
		// 			}
		// 		}
		// #pragma endregion
		//
		// #pragma region Rotate Mesh
		// 		if (IsRightRoll)
		// 		{
		// 			JetRoot->AddRelativeRotation(RotateRollValue);
		// 		}
		// 		else if (IsLeftRoll)
		// 		{
		// 			JetRoot->AddRelativeRotation(RotateRollValue * -1);
		// 		}
		// 		else if (IsKeyUpPress)
		// 		{
		// 			// JetRoot->AddRelativeRotation(RotatePitchValue);
		// 			JetRoot->AddWorldRotation(RotatePitchValue* -1);
		// 		}
		// 		else if (IsKeyDownPress)
		// 		{
		// 			//JetRoot->AddRelativeRotation(RotatePitchValue * -1);
		// 			JetRoot->AddWorldRotation(RotatePitchValue);
		// 		}
		// #pragma endregion

#pragma region Jet Move
		ValueOfMoveForce += (GetAddTickSpeed() * 6);
		if (ValueOfMoveForce < 0)
			ValueOfMoveForce = 0;
		else if (ValueOfMoveForce > MaxValueOfMoveForce)
			ValueOfMoveForce = MaxValueOfMoveForce;

		if (IsEngineOn)
		{
			// Add Force
			FVector forceVec = JetArrow->GetForwardVector() * ValueOfMoveForce;
			FVector forceLoc = JetRoot->GetComponentLocation();
			if (JetRoot->IsSimulatingPhysics())
				JetRoot->AddForceAtLocation(forceVec , forceLoc);

			// Move Up & Down
			jetRot = JetArrow->GetRelativeRotation();
			float zRot = jetRot.Quaternion().Y * jetRot.Quaternion().W * ValueOfHeightForce * 10.f;
			JetRoot->AddForceAtLocation(FVector(0 , 0 , zRot) , HeightForceLoc);

			// 카메라 쉐이크
			// 활주로를 달리고 있을때가 intTriggerNum < 2 이다.
			if (intTriggerNum < 2 && ValueOfMoveForce > 0)
				CRPC_CameraShake();
		}
#pragma endregion


#pragma region LockOn
		IsLockOn();
#pragma endregion

#pragma region Flare Arrow Rotation Change
		if (CurrentWeapon == EWeapon::Flare)
		{
			int32 randRot = FMath::RandRange(-150 , -110);
			//LOG_SCREEN("%d" , randRot);
			FRotator newFlareRot = FRotator(randRot , 0 , 0);
			JetFlareArrow3->SetRelativeRotation(newFlareRot);
			JetFlareArrow2->SetRelativeRotation(newFlareRot);
		}
#pragma endregion
	}

	if (IsLocallyControlled())
	{
		ChangeBooster();

		//<><><>
#pragma region 고도계
		float CurrHeight = GetActorLocation().Z; // + HeightOfSea; // 고도 높이
		float CurrFeet = CurrHeight * 3.281 / 100; // cm to feet
		if (CurrFeet < 0)
			CurrFeet = 0;

		if (auto HUDui = Cast<UL_HUDWidget>(JetWidget->GetWidget()))
		{
			HUDui->UpdateHeightBar(CurrFeet);
		}
#pragma endregion

#pragma region 속도계
		// 100 = 1m, 1000000=1km, 1km = 0.539957 Note
		float km = ValueOfMoveForce / 1000000.f;
		int32 ValueOfMoveForceInNote = static_cast<int32>(km * 0.539957);
		if (auto HUDui = Cast<UL_HUDWidget>(JetWidget->GetWidget()))
		{
			HUDui->UpdateSpeedText(ValueOfMoveForceInNote);
		}
#pragma endregion

#pragma region Zoom In/Out
		if (JetCameraFPS->IsActive())
		{
			if (IsZoomIn)
			{
				// 현재 FieldOfView 수치와 줌 인 수치(25)를 Lerp해서 적용
				float currViewValue = JetCameraFPS->FieldOfView;
				float newViewValue = FMath::Lerp(currViewValue , ZoomInValue , DeltaTime);
				JetCameraFPS->SetFieldOfView(newViewValue);
			}

			if (IsZoomOut)
			{
				// 현재 FieldOfView 수치와 줌 아웃 수치(140)를 Lerp해서 적용
				float currViewValue = JetCameraFPS->FieldOfView;
				float newViewValue = FMath::Lerp(currViewValue , ZoomOutValue , DeltaTime);
				JetCameraFPS->SetFieldOfView(newViewValue);
			}
		}
#pragma endregion
	}

#pragma region Recover CameraArm Rotation
	if (!IsRotateTrigger)
	{
		if (JetCamera->IsActive())
		{
			FRotator TPSrot = JetSprintArm->GetRelativeRotation();
			//FRotator(-10 , 0 , 0)
			auto lerpTPSrot = FMath::Lerp(TPSrot , FRotator(-10 , 0 , 0) , DeltaTime);
			JetSprintArm->SetRelativeRotation(lerpTPSrot);
		}
		else
		{
			// FRotator FPSrot = JetSprintArmFPS->GetRelativeRotation();
			// //FRotator(-30 , 0 , 0)
			// auto lerpFPSrot = FMath::Lerp(FPSrot , FRotator(-30 , 0 , 0) , DeltaTime);
			// JetSprintArmFPS->SetRelativeRotation(lerpFPSrot);
		}
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
		TEXT("Connection : %s\nPawn Name : %s\nOwner Name : %s\nLocal Role : %s\nRemote Role : %s") , *conStr ,
		*GetName() , *ownerName ,
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

			FRotator SpawnRotation = FRotator((LockOnTarget->GetActorLocation() - GetActorLocation()).Rotation().Yaw ,
			                                  0 , 0); // Update this with the desired rotation for the missile
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
}

void AL_Viper::ServerRPCFlare_Implementation(AActor* newOwner)
{
	if (FlareFactory)
	{
		if (FlareCurCnt > 0)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = newOwner;
			// 던질 위치 계산(캐릭터 위치에서 위로 조정)
			FVector SpawnLocation = JetFlareArrow3->GetComponentLocation();
			// 던질 각도
			FRotator SpawnRotation = JetMesh->GetComponentRotation();
			// FlareFactory 이용해서 수류탄 스폰
			AL_Flare* Flare1 = GetWorld()->SpawnActor<AL_Flare>(FlareFactory , SpawnLocation , SpawnRotation ,
			                                                    SpawnParams);

			if (Flare1)
			{
				// Flare1->SetOwner(newOwner);
				FlareCurCnt--;
			}

			// 던질 위치 계산(캐릭터 위치에서 위로 조정)
			SpawnLocation = JetFlareArrow2->GetComponentLocation();
			// 던질 각도
			//SpawnRotation = FRotator::ZeroRotator;
			// FlareFactory 이용해서 수류탄 스폰
			AL_Flare* Flare2 = GetWorld()->SpawnActor<AL_Flare>(FlareFactory , SpawnLocation , SpawnRotation ,
			                                                    SpawnParams);

			if (Flare2)
			{
				// Flare2->SetOwner(newOwner);
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
	//LockOnTarget = nullptr;
	AActor* searchTarget = nullptr;
	FVector Start = JetMesh->GetComponentLocation();
	FVector ForwardVector = JetMesh->GetForwardVector();
	FVector DownVector = JetMesh->GetUpVector() * -1;

	TArray<AActor*> Overlaps;
	TArray<FHitResult> OutHit;
	for (int i = 0; i < RangeCnt; i++)
	{
		Diametr *= 2.f;
		Start += (ForwardVector * Diametr / 4) + (DownVector * Diametr / 2);
		if (UKismetSystemLibrary::SphereTraceMulti(GetWorld() , Start , Start , Diametr / 2.f , TraceTypeQuery1 ,
		                                           false , Overlaps , EDrawDebugTrace::ForOneFrame , OutHit , true))
		{
			for (auto hit : OutHit)
			{
				if (auto mai = Cast<IJ_MissionActorInterface>(hit.GetActor()))
				{
					if (!LockOnTarget)
						ClientRPCLockOnSound(this);
					searchTarget = hit.GetActor();
				}
				// if (hit.GetActor()->ActorHasTag("target"))
				// {
				// 	
				// }
			}
		}
	}

	MulticastRPCLockOn(searchTarget);
	ClientRPCSetLockOnUI(this , searchTarget);

	Diametr = 30.f;
}

void AL_Viper::MulticastRPCLockOn_Implementation(AActor* target)
{
	LockOnTarget = target;

	// LOG_S(Warning , TEXT("Viper Name : %s, LockOnTarget Name : %s") , *GetName() ,
	// 					  *LockOnTarget->GetName());
}

void AL_Viper::ClientRPCLockOnSound_Implementation(AL_Viper* CurrentViper)
{
	CurrentViper->PlayLockOnSound();
}

void AL_Viper::ClientRPCSetLockOnUI_Implementation(AL_Viper* CurrentViper , AActor* target)
{
	if (target && TargetUIActorFac)
	{
		if (TargetUIActorFac)
		{
			if (!TargetActor)
			{
				FActorSpawnParameters SpawnParams;
				SpawnParams.Owner = this;
				SpawnParams.Instigator = GetInstigator();

				FVector TargetLocation = target->GetActorLocation() + FVector(0 , 0 , 100);
				FRotator TargetRotation = (GetActorLocation() - TargetLocation).Rotation();

				TargetActor = GetWorld()->SpawnActor<AL_Target>(TargetUIActorFac , TargetLocation , TargetRotation ,
				                                                SpawnParams);
			}
		}
	}
	else
	{
		if (TargetActor)
		{
			TargetActor->Destroy();
			TargetActor = nullptr;
			//TargetActor->F_Destroy();
		}
	}
}

void AL_Viper::PlayLockOnSound()
{
	if (LockOnSound)
		UGameplayStatics::PlaySound2D(this , LockOnSound);
}

void AL_Viper::CreateDumyComp()
{
	DummyMICMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DummyMICMesh"));
	DummyMICMesh->SetRelativeLocationAndRotation(FVector(0 , 0 , -.9) , FRotator(30 , 0 , 0));
	DummyMICMesh->SetupAttachment(JetMic);

	DummyEngineGenerMesh1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DummyEngineGenMesh1"));
	DummyEngineGenerMesh1->SetupAttachment(JetEngineGen);
	DummyEngineGenerMesh1->SetRelativeLocationAndRotation(FVector(0 , 0 , -.9) , FRotator(30 , 0 , 0));

	DummyEngineGenerMesh2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DummyEngineGenMesh2"));
	DummyEngineGenerMesh2->SetupAttachment(JetEngineGen2);
	DummyEngineGenerMesh2->SetRelativeLocationAndRotation(FVector(0 , 0 , -.9) , FRotator(30 , 0 , 0));

	DummyEngineControlMesh1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DummyEngineControlMesh1"));
	DummyEngineControlMesh1->SetupAttachment(JetEngineControl);
	DummyEngineControlMesh1->SetRelativeLocationAndRotation(FVector(0 , 0 , -.9) , FRotator(30 , 0 , 0));

	DummyEngineControlMesh2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DummyEngineControlMesh2"));
	DummyEngineControlMesh2->SetupAttachment(JetEngineControl2);
	DummyEngineControlMesh2->SetRelativeLocationAndRotation(FVector(0 , 0 , -.9) , FRotator(30 , 0 , 0));

	DummyJFSMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DummyJFSMesh"));
	DummyJFSMesh->SetupAttachment(JetFuelStarter);
	DummyJFSMesh->SetRelativeLocationAndRotation(FVector(0 , 0 , -.9) , FRotator(30 , 0 , 0));

	DummyEngineMasterMesh1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DummyEngineMasterMesh1"));
	DummyEngineMasterMesh1->SetupAttachment(JetEngineMaster);
	DummyEngineMasterMesh1->SetRelativeLocationAndRotation(FVector(0 , 0 , -.9) , FRotator(30 , 0 , 0));

	DummyEngineMasterMesh2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DummyEngineMasterMesh2"));
	DummyEngineMasterMesh2->SetupAttachment(JetEngineMaster2);
	DummyEngineMasterMesh2->SetRelativeLocationAndRotation(FVector(0 , 0 , -.9) , FRotator(30 , 0 , 0));

	DummyJFSHandleMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DummyJFSHandleMesh"));
	DummyJFSHandleMesh->SetupAttachment(JetJFSHandle);
	DummyJFSHandleMesh->SetRelativeLocation(FVector(-550 , -33 , -253));

	DummyThrottleMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DummyThrottleMesh"));
	DummyThrottleMesh->SetupAttachment(JetFirstEngine);
	DummyThrottleMesh->SetRelativeScale3D(FVector(1.5 , 1.5 , 1));
	DummyThrottleMesh->SetRelativeLocation(FVector(0 , 0 , -8));

	DummyCanopyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DummyCanopyMesh"));
	DummyCanopyMesh->SetupAttachment(JetCanopy);
	DummyCanopyMesh->SetRelativeLocation(FVector(-521 , -40 , -268));

	DummyJFSBreakHold = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DummyJFSBreakHold"));
	DummyJFSBreakHold->SetupAttachment(JetBreakHold);
	DummyJFSBreakHold->SetRelativeLocationAndRotation(FVector(0 , 0 , -.9) , FRotator(30 , 0 , 0));
}

void AL_Viper::SetAccelGear()
{
	// 기어 변동 구간
	// 0%, 50%, 90%
	auto currAccelGear = JetFirstEngine->GetRelativeLocation().X;
	auto currValue = currAccelGear - ThrottleOffLoc.X;
	auto SizeValue = ThrottleMaxLoc.X - ThrottleOffLoc.X;
	auto per = currValue / SizeValue * 100;
	if (per <= 5)
	{
		// IsEngineOn = false;
		AccelGear = 0;
	}
	else if (per <= 50)
	{
		// IsEngineOn = true;
		AccelGear = 1;
	}
	else if (per <= 90)
	{
		// IsEngineOn = true;
		AccelGear = 2;
	}
	else
	{
		// IsEngineOn = true;
		AccelGear = 3;
	}
}

void AL_Viper::PerformLineTrace()
{
	FVector WorldLocation , WorldDirection;
	if (auto pc = GetWorld()->GetFirstPlayerController())
	{
		if (pc->DeprojectMousePositionToWorld(WorldLocation , WorldDirection))
		{
			// 마우스 위치에서 라인 트레이스 시작
			FVector Start = WorldLocation;
			FVector End = Start + (WorldDirection * 10000.0f);

			FHitResult HitResult;
			FCollisionQueryParams Params;

			// 라인 트레이스 수행
			if (GetWorld()->LineTraceSingleByChannel(HitResult , Start , End , ECC_Visibility , Params))
			{
				if (HitResult.GetComponent()->ComponentHasTag("Canopy"))
				{
					BackMoveCanopyHandle();
					//LOG_SCREEN("캐노피 우클릭");
					// 디버그용 라인 시각화
					//DrawDebugLine(GetWorld() , Start , End , FColor::Green , false , 2.0f , 0 , 2.0f);
				}
			}
		}
	}
}

void AL_Viper::BackMoveCanopyHandle()
{
	CRPC_PlaySwitchSound(JetCanopy->GetComponentLocation());

	auto currLoc = JetCanopy->GetRelativeLocation();

	if (FVector::Dist(currLoc , CanopyHoldLoc) <= 1)
	{
		JetCanopy->SetRelativeLocation(CanopyCloseLoc);
		iCanopyNum = 2;
	}
	else if (FVector::Dist(currLoc , CanopyCloseLoc) <= 1)
	{
		JetCanopy->SetRelativeLocation(CanopyNormalLoc);
		iCanopyNum = 1;
	}
	else if (FVector::Dist(currLoc , CanopyNormalLoc) <= 1)
	{
		JetCanopy->SetRelativeLocation(CanopyOpenLoc);
		iCanopyNum = 0;
	}
}

void AL_Viper::ServerRPC_Canopy_Implementation(bool bOpen)
{
	if (bOpen)
	{
		// 캐노피가 열린다.
		float newPitch = CanopyPitch + CanopyRotatePitchValue;
		newPitch = FMath::Clamp(newPitch , 0.f , 80.f);
		CanopyPitch = newPitch;
		LOG_S(Warning , TEXT("Open %f") , newPitch);
	}
	else
	{
		// 캐노피가 닫힌다.
		float newPitch = CanopyPitch - CanopyRotatePitchValue;
		newPitch = FMath::Clamp(newPitch , 0.f , 80.f);
		CanopyPitch = newPitch;
		LOG_S(Warning , TEXT("Close %f") , newPitch);
	}
}

void AL_Viper::ServerRPC_Wheel_Implementation()
{
	float DeltaTime = GetWorld()->DeltaTimeSeconds;
	float nowValue = FMath::Lerp(FrontWheel , 1 , DeltaTime);

	if (nowValue >= 0.9)
		nowValue = 1;

	FrontWheel = nowValue;
	RearWheel = nowValue;
}

void AL_Viper::StartVoiceChat()
{
	GetController<AJ_MissionPlayerController>()->StartTalking();
}

void AL_Viper::StopVoiceChat()
{
	GetController<AJ_MissionPlayerController>()->StopTalking();
}

void AL_Viper::OnMyMemberReFresh()
{
}

void AL_Viper::ServerRPC_SetCurrentReadyMem_Implementation()
{
	ReadyMemeberCnt++;
	MultiRPC_SetCurrentReadyMem(ReadyMemeberCnt);
}

void AL_Viper::MultiRPC_SetCurrentReadyMem_Implementation(int32 cnt)
{
	ReadyMemeberCnt++;
	WaitingForStartUI->SetMem(ReadyMemeberCnt);
}

void AL_Viper::ReadyAllMembers()
{
	if (WaitingForStartUI)
	{
		WaitingForStartUI->RemoveFromParent();
	}
	IsEngineOn = true;
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
}

#pragma region KHS Works
void AL_Viper::ServerRPC_SetConnectedPlayerNames_Implementation(const FString& newName)
{
	auto KGameState = CastChecked<AK_GameState>(UGameplayStatics::GetGameState(GetWorld()));
	if (!KGameState)
	{
		LOG_S(Warning , TEXT("GameState doesn't exist"));
	}
	LOG_S(Warning , TEXT("MyUserID : %s") , *newName);

	//GameState의 ConnectedPlayerNames 배열에 자신의 ID Set(Replicated)
	KGameState->SetConnectedPlayerNames(newName);

	//GameState에 업데이트된 배열 가져옴
	TArray<FString> temp = KGameState->GetConnectedPlayernames();

	//로그출력
	for (auto s : temp)
	{
		LOG_S(Warning , TEXT("The Name in PlayerList : %s") , *s);
	}

	//월드에 존재하는 PlayterController배열
	TArray<AK_PlayerController*> allPC;
	//배열에 GameState에 있는 PlayerArray에 접근해서 모든 PC담기
	Algo::Transform(KGameState->PlayerArray , allPC , [](TObjectPtr<APlayerState> PS)
	{
		check(PS);
		auto* tempPC = CastChecked<AK_PlayerController>(PS->GetPlayerController());
		check(tempPC);
		return tempPC;
	});

	//PC배열을 검사해서 모든 pc에서 CRPC함수 실행
	for (auto localpc : allPC)
	{
		auto me = Cast<AL_Viper>(localpc->GetPawn());
		check(me);
		//CRPC로 업데이트된 배열을 클라이언트들의 GameState에 업데이트
		me->ClientRPC_SetConnectedPlayerNames(temp);
	}
}

void AL_Viper::ClientRPC_SetConnectedPlayerNames_Implementation(const TArray<FString>& newNames)
{
	//UI를 가져와서 현재의 GameState값 기준으로 SetPlayerList하도록 하기
	AK_PlayerController* LocalPlayerController = Cast<AK_PlayerController>(GetWorld()->GetFirstPlayerController());
	AK_PlayerController* MyPlayerController = Cast<AK_PlayerController>(GetOwner());

	//First PC가 내 Owner와 동일하다면
	if (LocalPlayerController == MyPlayerController)
	{
		UK_StandbyWidget* LocalStandbyWidget = Cast<UK_StandbyWidget>(LocalPlayerController->StandbyUI);
		if (LocalStandbyWidget)
		{
			//타이머로 클라이언트가 입장후 Replicated가 완료되는 시간 기다리기
			FTimerHandle tempHandle;
			GetWorld()->GetTimerManager().SetTimer(tempHandle , [this , LocalStandbyWidget]()
			{
				LocalStandbyWidget->SetPlayerList();
			} , 1.0f , false);
		}
	}
}
#pragma endregion

void AL_Viper::CRPC_AudioControl_Implementation(bool bStart , int32 idx)
{
	if (bStart)
	{
		if (JetAudio && JetAudio->GetSound())
		{
			JetAudio->SetIntParameter("JetSoundIdx" , idx);
			JetAudio->Play(0.f);
		}
	}
	else
	{
		if (JetAudio && JetAudio->GetSound())
			JetAudio->Stop();
	}
}

void AL_Viper::Call_CRPC_MissileImpact(FVector ImpactLoc)
{
	auto KGameState = CastChecked<AK_GameState>(UGameplayStatics::GetGameState(GetWorld()));
	if (!KGameState)
	{
		LOG_S(Warning , TEXT("GameState doesn't exist"));
	}

	//월드에 존재하는 PlayerController배열
	TArray<AK_PlayerController*> allPC;
	//배열에 GameState에 있는 PlayerArray에 접근해서 모든 PC담기
	Algo::Transform(KGameState->PlayerArray , allPC , [](TObjectPtr<APlayerState> PS)
	{
		check(PS);
		auto* tempPC = CastChecked<AK_PlayerController>(PS->GetPlayerController());
		check(tempPC);
		return tempPC;
	});

	for (auto localpc : allPC)
	{
		auto me = Cast<AL_Viper>(localpc->GetPawn());
		check(me);
		//CRPC로 업데이트된 배열을 클라이언트들의 GameState에 업데이트
		me->CRPC_MissileImpact(ImpactLoc);
	}
}

void AL_Viper::CRPC_MissileImpact_Implementation(FVector ImpactLoc)
{
	FVector VFXSpawnLoc = ImpactLoc + FVector::UpVector * 10000.f;
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld() , DistroyVFX , VFXSpawnLoc);
	UGameplayStatics::PlaySoundAtLocation(this , ImpactSound , GetActorLocation());
}

void AL_Viper::CRPC_PlaySwitchSound_Implementation(FVector SoundLoc)
{
	if (SwitchSound)
		UGameplayStatics::PlaySoundAtLocation(this , SwitchSound , SoundLoc);
}

void AL_Viper::CRPC_CameraShake_Implementation()
{
	if (LoadCameraShake)
		UGameplayStatics::PlayWorldCameraShake(GetWorld() , LoadCameraShake , GetActorLocation() , 300.f , 700.f);
}

void AL_Viper::F_ThrottleButton8Started(const struct FInputActionValue& value)
{
	bVoice = !bVoice;
	if (bVoice)
		StartVoiceChat();
	else
		StopVoiceChat();
}

void AL_Viper::F_ThrottleButton15Started(const struct FInputActionValue& value)
{
	if (JetJFSHandle)
		OnMyJFSHandle1Clicked(JetJFSHandle , lMouse);
}

void AL_Viper::F_ThrottleButton34Started(const struct FInputActionValue& value)
{
	if (JetMic && !bMIC)
		OnMyMicClicked(JetMic , lMouse);
}

void AL_Viper::F_ThrottleButton35Started(const struct FInputActionValue& value)
{
	if (JetMic && bMIC)
		OnMyMicClicked(JetMic , lMouse);
}

void AL_Viper::F_ThrottleButton36Started(const struct FInputActionValue& value)
{
	if (JetEngineGen && !bEngineGen1)
		OnMyEngineGen1Clicked(JetEngineGen , lMouse);
	FTimerHandle hnd;
	GetWorld()->GetTimerManager().SetTimer(hnd , [&]()
	{
		if (JetEngineGen2 && !bEngineGen2)
			OnMyEngineGen2Clicked(JetEngineGen2 , lMouse);
	} , switchLate , false);
}

void AL_Viper::F_ThrottleButton37Started(const struct FInputActionValue& value)
{
	if (JetEngineGen && bEngineGen1)
		OnMyEngineGen1Clicked(JetEngineGen , lMouse);
	FTimerHandle hnd;
	GetWorld()->GetTimerManager().SetTimer(hnd , [&]()
	{
		if (JetEngineGen2 && bEngineGen2)
			OnMyEngineGen2Clicked(JetEngineGen2 , lMouse);
	} , switchLate , false);
}

void AL_Viper::F_ThrottleButton38Started(const struct FInputActionValue& value)
{
	if (JetEngineControl && !bEngineControl1)
		OnMyEngineControlClicked(JetEngineControl , lMouse);
	FTimerHandle hnd;
	GetWorld()->GetTimerManager().SetTimer(hnd , [&]()
	{
		if (JetEngineControl2 && !bEngineControl2)
			OnMyEngineControl2Clicked(JetEngineControl2 , lMouse);
	} , switchLate , false);
}

void AL_Viper::F_ThrottleButton39Started(const struct FInputActionValue& value)
{
	if (JetEngineControl && bEngineControl1)
		OnMyEngineControlClicked(JetEngineControl , lMouse);
	FTimerHandle hnd;
	GetWorld()->GetTimerManager().SetTimer(hnd , [&]()
	{
		if (JetEngineControl2 && bEngineControl2)
			OnMyEngineControl2Clicked(JetEngineControl2 , lMouse);
	} , switchLate , false);
}

void AL_Viper::F_ThrottleButton40Started(const struct FInputActionValue& value)
{
	if (JetEngineMaster && !bEngineMaster1)
		OnMyEngineMaster1Clicked(JetEngineMaster , lMouse);
	FTimerHandle hnd;
	GetWorld()->GetTimerManager().SetTimer(hnd , [&]()
	{
		if (JetEngineMaster2 && !bEngineMaster2)
			OnMyEngineMaster2Clicked(JetEngineMaster2 , lMouse);
	} , switchLate , false);
}

void AL_Viper::F_ThrottleButton41Started(const struct FInputActionValue& value)
{
	if (JetEngineMaster && bEngineMaster1)
		OnMyEngineMaster1Clicked(JetEngineMaster , lMouse);
	FTimerHandle hnd;
	GetWorld()->GetTimerManager().SetTimer(hnd , [&]()
	{
		if (JetEngineMaster2 && bEngineMaster2)
			OnMyEngineMaster2Clicked(JetEngineMaster2 , lMouse);
	} , switchLate , false);
}

void AL_Viper::F_ThrottleButton42Started(const struct FInputActionValue& value)
{
	if (JetFuelStarter && !bJFS)
		OnMyJetFuelStarterClicked(JetFuelStarter , lMouse);
}

void AL_Viper::F_ThrottleButton43Started(const struct FInputActionValue& value)
{
	if (JetFuelStarter && bJFS)
		OnMyJetFuelStarterClicked(JetFuelStarter , lMouse);
}

void AL_Viper::F_ThrottleButton46Started(const struct FInputActionValue& value)
{
	if (JetBreakHold && !bBreakHold)
		OnMyBreakHoldClicked(JetBreakHold , lMouse);
}

void AL_Viper::F_ThrottleButton47Started(const struct FInputActionValue& value)
{
	if (JetBreakHold && bBreakHold)
		OnMyBreakHoldClicked(JetBreakHold , lMouse);
}

void AL_Viper::F_ThrottleAxis4(const struct FInputActionValue& value)
{
	float data = value.Get<float>(); // 갱신용 데이터
	float newData = data; // 이동용 데이터
	LOG_S(Warning , TEXT("F_ThrottleAxis4 : %f") , data);

	if (DeviceThrottleCurrentValue > data)
	{
		// 스로틀 Down
		if (intTriggerNum == 0)
		{
			if (newData > .25f)
				return;
		}
		else if (intTriggerNum == 1)
		{
			if (newData > .8f)
				return;
		}

		auto SizeValue = ThrottleMaxLoc.X - ThrottleOffLoc.X;
		auto moveValue = SizeValue * newData;
		JetFirstEngine->
			SetRelativeLocation(FVector(ThrottleOffLoc.X + moveValue , ThrottleOffLoc.Y , ThrottleOffLoc.Z));
	}
	else
	{
		if (!bFirstEngine)
			bFirstEngine = true;

		if (StartScenario.size() > 0 && StartScenario.front() == "Throttle")
		{
			StartScenario.pop();
			DummyThrottleMesh->SetRenderCustomDepth(false);
			DummyThrottleMesh->CustomDepthStencilValue = 0;
		}

		// 스로틀 up
		if (intTriggerNum == 0)
		{
			float per = .25f;
			newData = UKismetMathLibrary::FClamp(newData , .0f , per);
		}
		else if (intTriggerNum == 1)
		{
			float per = .8f;
			newData = UKismetMathLibrary::FClamp(newData , .0f , per);
		}

		auto SizeValue = ThrottleMaxLoc.X - ThrottleOffLoc.X;
		auto moveValue = SizeValue * newData;
		JetFirstEngine->
			SetRelativeLocation(FVector(ThrottleOffLoc.X + moveValue , ThrottleOffLoc.Y , ThrottleOffLoc.Z));
	}
	DeviceThrottleCurrentValue = data;
}

void AL_Viper::F_ThrottleAxis6(const struct FInputActionValue& value)
{
	float data = value.Get<float>();
	// LOG_S(Warning , TEXT("F_ThrottleAxis6 : %f") , data);
	if (data > .8f)
	{
		// 잠금
		JetCanopy->SetRelativeLocation(CanopyHoldLoc);
		iCanopyNum = 3;
	}
	else if (data > .6f)
	{
		// 닫기
		JetCanopy->SetRelativeLocation(CanopyCloseLoc);
		iCanopyNum = 2;
	}
	else if (data > .3f)
	{
		// 중립
		JetCanopy->SetRelativeLocation(CanopyNormalLoc);
		iCanopyNum = 1;
	}
	else
	{
		// 열기
		JetCanopy->SetRelativeLocation(CanopyOpenLoc);
		iCanopyNum = 0;
	}
}

void AL_Viper::F_StickButton1Started(const struct FInputActionValue& value)
{
	// auto b = value.Get<bool>();
	// LOG_S(Warning , TEXT("F_StickButton1Started : %s") , b?*FString("true"):*FString("false"));
	CurrentWeapon = static_cast<EWeapon>((static_cast<int32>(CurrentWeapon) + 1) % static_cast<int32>(EWeapon::Max));
}

void AL_Viper::F_StickButton2Started(const struct FInputActionValue& value)
{
	// auto b = value.Get<bool>();
	// LOG_S(Warning , TEXT("F_StickButton2Started : %s") , b?*FString("true"):*FString("false"));
	if (CurrentWeapon == EWeapon::Missile)
		ServerRPCMissile(this);
	else if (CurrentWeapon == EWeapon::Flare)
		ServerRPCFlare(this);
}

void AL_Viper::F_StickButton5Started(const struct FInputActionValue& value)
{
	// auto b = value.Get<bool>();
	// LOG_S(Warning , TEXT("F_StickButton5Started : %s") , b?*FString("true"):*FString("false"));
	if (JetCamera && JetCamera->IsActive())
	{
		JetCamera->SetActive(false);
		if (JetCameraFPS)
		{
			if (JetPostProcess && JetPostProcess->Settings.WeightedBlendables.Array.Num() > 0)
				JetPostProcess->Settings.WeightedBlendables.Array[0].Weight = 1;
			JetCameraFPS->SetActive(true);
		}
	}
	else
	{
		if (JetCamera)
		{
			JetCamera->SetActive(true);
			if (JetCameraFPS)
			{
				if (JetPostProcess && JetPostProcess->Settings.WeightedBlendables.Array.Num() > 0)
					JetPostProcess->Settings.WeightedBlendables.Array[0].Weight = 0;
				JetCameraFPS->SetActive(false);
			}
		}
	}
}

void AL_Viper::F_StickButton11Started(const struct FInputActionValue& value)
{
	// auto b = value.Get<bool>();
	// LOG_S(Warning , TEXT("F_StickButton11Started : %s") , b?*FString("true"):*FString("false"));
	if (!IsZoomOut)
		IsZoomIn = true;
}

void AL_Viper::F_StickButton11Completed(const struct FInputActionValue& value)
{
	// auto b = value.Get<bool>();
	// LOG_S(Warning , TEXT("F_StickButton11Completed : %s") , b?*FString("true"):*FString("false"));
	// 중복 키입력 방지용
	IsZoomIn = false;
}

void AL_Viper::F_StickButton13Started(const struct FInputActionValue& value)
{
	// auto b = value.Get<bool>();
	// LOG_S(Warning , TEXT("F_StickButton13Started : %s") , b?*FString("true"):*FString("false"));
	if (!IsZoomIn)
		IsZoomOut = true;
}

void AL_Viper::F_StickButton13Completed(const struct FInputActionValue& value)
{
	// auto b = value.Get<bool>();
	// LOG_S(Warning , TEXT("F_StickButton13Completed : %s") , b?*FString("true"):*FString("false"));
	IsZoomOut = false;
}

void AL_Viper::F_StickAxis1(const struct FInputActionValue& value)
{
	float data = value.Get<float>();
	data = FMath::RoundToFloat(data * 1000.0f) / 1000.0f;
	FString strData = FString::Printf(TEXT("%.3f") , data);
	//LOG_S(Warning , TEXT("F_StickAxis1 : %s") , *strData);

	float X = 0;
	float Y = 0;
	if (strData.Equals("3.286"))
	{
		IsRotateTrigger = false;
		return;
	}
	else if (strData.Equals("-1.000"))
		Y = 1;
	else if (strData.Equals("-0.714"))
	{
		X = 1;
		Y = 1;
	}
	else if (strData.Equals("-0.429"))
		X = 1;
	else if (strData.Equals("-0.143"))
	{
		X = 1;
		Y = -1;
	}
	else if (strData.Equals("0.143"))
		Y = -1;
	else if (strData.Equals("0.429"))
	{
		X = -1;
		Y = -1;
	}
	else if (strData.Equals("0.714"))
		X = -1;
	else if (strData.Equals("1.000"))
	{
		X = -1;
		Y = 1;
	}
	IsRotateTrigger = true;

	if (JetCamera->IsActive())
	{
		FRotator TPSrot = JetSprintArm->GetRelativeRotation();
		float newTpsYaw = TPSrot.Yaw + X;
		float newTpsPitch = TPSrot.Pitch + Y;
		newTpsYaw = UKismetMathLibrary::FClamp(newTpsYaw , -360.f , 360.f);
		newTpsPitch = UKismetMathLibrary::FClamp(newTpsPitch , -80.f , 80.f);
		JetSprintArm->SetRelativeRotation(FRotator(newTpsPitch , newTpsYaw , 0));
	}
	else
	{
		FRotator FPSrot = JetSprintArmFPS->GetRelativeRotation();
		float newFpsYaw = FPSrot.Yaw + X;
		float newFpsPitch = FPSrot.Pitch + Y;
		newFpsYaw = UKismetMathLibrary::FClamp(newFpsYaw , -100.f , 100.f);
		newFpsPitch = UKismetMathLibrary::FClamp(newFpsPitch , -100.f , 100.f);
		JetSprintArmFPS->SetRelativeRotation(FRotator(newFpsPitch , newFpsYaw , 0));
	}
}

void AL_Viper::F_StickAxis2(const struct FInputActionValue& value)
{
	// Up(1), Down(-1)
	float data = value.Get<float>();
	StickPitchAngle = data * -1;
	// LOG_S(Warning , TEXT("F_StickAxis2 : %f") , data);
}

void AL_Viper::F_StickAxis3(const struct FInputActionValue& value)
{
	// Left(-1), Right(1)
	float data = value.Get<float>();
	StickRollAngle = data * -1;
	// LOG_S(Warning , TEXT("F_StickAxis3 : %f") , data);
	if ((StickRollAngle > 0.6f || StickRollAngle < -0.6f) && (StickPitchAngle > 0.6f || StickPitchAngle < -0.6f))
	{
		StickRollAngle = StickRollAngle * MaxRotationAngle / 3.f;
		StickPitchAngle = StickPitchAngle * MaxRotationAngle / 3.f;
	}
	else
	{
		StickRollAngle = StickRollAngle * MaxRotationAngle / 5.f;
		StickPitchAngle = StickPitchAngle * MaxRotationAngle / 6.f;
	}

	// Roll과 Pitch를 쿼터니언 회전으로 변환
	FQuat RollRotation = FQuat(FVector(1 , 0 , 0) , FMath::DegreesToRadians(StickRollAngle));
	FQuat PitchRotation = FQuat(FVector(0 , 1 , 0) , FMath::DegreesToRadians(StickPitchAngle));

	// 목표 회전 설정 (RootComponent를 기준으로)
	QuatTargetRotation = QuatCurrentRotation * RollRotation * PitchRotation;

	StickRollAngle = 0.f;
	StickPitchAngle = 0.f;
}

void AL_Viper::VRSticAxis(const struct FInputActionValue& value)
{
	auto data = value.Get<FVector2D>();
	VRStickCurrentPitchValue = data.X;
	VRStickCurrentRollValue = data.Y;
	float VRStickRollAngle=0.f;
	float VRStickPitchAngle=0.f;
	// LOG_S(Warning , TEXT("F_StickAxis3 : %f") , data);
	if ((VRStickRollAngle > VRStickMaxThreshold || VRStickRollAngle < VRStickMinThreshold) && (VRStickPitchAngle > VRStickMaxThreshold || VRStickPitchAngle < VRStickMinThreshold))
	{
		VRStickRollAngle = VRStickRollAngle * MaxRotationAngle / VRStickBankRollDiv;
		VRStickPitchAngle = VRStickPitchAngle * MaxRotationAngle / VRStickBankPitchDiv;
	}
	else
	{
		VRStickRollAngle = VRStickRollAngle * MaxRotationAngle / VRStickkRollDiv;
		VRStickPitchAngle = VRStickPitchAngle * MaxRotationAngle /VRStickPitchDiv;
	}

	// Roll과 Pitch를 쿼터니언 회전으로 변환
	FQuat RollRotation = FQuat(FVector(1 , 0 , 0) , FMath::DegreesToRadians(VRStickRollAngle));
	FQuat PitchRotation = FQuat(FVector(0 , 1 , 0) , FMath::DegreesToRadians(VRStickPitchAngle));

	// 목표 회전 설정 (RootComponent를 기준으로)
	QuatTargetRotation = QuatCurrentRotation * RollRotation * PitchRotation;

	VRStickCurrentPitchValue = 0.f;
	VRStickCurrentRollValue = 0.f;
}
