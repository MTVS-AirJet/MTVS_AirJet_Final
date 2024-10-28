#include "LHJ/L_Viper.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "MTVS_AirJet_Final.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "Camera/CameraComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Components/PostProcessComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "JBS/J_MissionActorInterface.h"
#include "KHS/K_CesiumTeleportBox.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "LHJ/L_Flare.h"
#include "LHJ/L_HUDWidget.h"
#include "LHJ/L_Missile.h"
#include "LHJ/L_RoadTrigger.h"
#include "Net/UnrealNetwork.h"


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
	JetArrow->SetHiddenInGame(false); // For Test

	JetFirstEngine = CreateDefaultSubobject<UBoxComponent>(TEXT("JetFirstEngine"));
	JetFirstEngine->SetupAttachment(JetMesh);
	JetFirstEngine->SetRelativeScale3D(FVector(.1f));
	JetFirstEngine->SetRelativeLocation(FVector(385 , -28 , 237));
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

	JetEngineGen = CreateDefaultSubobject<UBoxComponent>(TEXT("JetEngineGen"));
	JetEngineGen->SetupAttachment(JetMesh);
	JetEngineGen->SetRelativeScale3D(FVector(.1f , .1f , .2f));
	JetEngineGen->SetRelativeLocation(FVector(410 , -7 , 280));
	JetEngineGen->SetGenerateOverlapEvents(true);
	JetEngineGen->OnClicked.AddDynamic(this , &AL_Viper::OnMyEngineGen1Clicked);
	JetEngineGen->SetHiddenInGame(false); // For Test

	JetEngineGen2 = CreateDefaultSubobject<UBoxComponent>(TEXT("JetEngineGen2"));
	JetEngineGen2->SetupAttachment(JetMesh);
	JetEngineGen2->SetRelativeScale3D(FVector(.1f , .1f , .2f));
	JetEngineGen2->SetRelativeLocation(FVector(410 , 0 , 280));
	JetEngineGen2->SetGenerateOverlapEvents(true);
	JetEngineGen2->OnClicked.AddDynamic(this , &AL_Viper::OnMyEngineGen2Clicked);
	JetEngineGen2->SetHiddenInGame(false); // For Test

	JetEngineControl = CreateDefaultSubobject<UBoxComponent>(TEXT("JetEngineControl"));
	JetEngineControl->SetupAttachment(JetMesh);
	JetEngineControl->SetRelativeScale3D(FVector(.1f , .1f , .2f));
	JetEngineControl->SetRelativeLocation(FVector(410 , 7 , 280));
	JetEngineControl->SetGenerateOverlapEvents(true);
	JetEngineControl->OnClicked.AddDynamic(this , &AL_Viper::OnMyEngineControlClicked);
	JetEngineControl->SetHiddenInGame(false); // For Test

	JetEngineControl2 = CreateDefaultSubobject<UBoxComponent>(TEXT("JetEngineControl2"));
	JetEngineControl2->SetupAttachment(JetMesh);
	JetEngineControl2->SetRelativeScale3D(FVector(.1f , .1f , .2f));
	JetEngineControl2->SetRelativeLocation(FVector(410 , 15 , 280));
	JetEngineControl2->SetGenerateOverlapEvents(true);
	JetEngineControl2->OnClicked.AddDynamic(this , &AL_Viper::OnMyEngineControl2Clicked);
	JetEngineControl2->SetHiddenInGame(false); // For Test

	JetFuelStarter = CreateDefaultSubobject<UBoxComponent>(TEXT("JetFuelStarter"));
	JetFuelStarter->SetupAttachment(JetMesh);
	JetFuelStarter->SetRelativeScale3D(FVector(.1f , .1f , .2f));
	JetFuelStarter->SetRelativeLocation(FVector(410 , 25 , 280));
	JetFuelStarter->SetGenerateOverlapEvents(true);
	JetFuelStarter->OnClicked.AddDynamic(this , &AL_Viper::OnMyJetFuelStarterClicked);
	JetFuelStarter->SetHiddenInGame(false); // For Test

	JetEngineMaster = CreateDefaultSubobject<UBoxComponent>(TEXT("JetEngineMaster"));
	JetEngineMaster->SetupAttachment(JetMesh);
	JetEngineMaster->SetRelativeScale3D(FVector(.1f , .1f , .2f));
	JetEngineMaster->SetRelativeLocation(FVector(410 , 7 , 260));
	JetEngineMaster->SetGenerateOverlapEvents(true);
	JetEngineMaster->OnClicked.AddDynamic(this , &AL_Viper::OnMyEngineMaster1Clicked);
	JetEngineMaster->SetHiddenInGame(false); // For Test

	JetEngineMaster2 = CreateDefaultSubobject<UBoxComponent>(TEXT("JetEngineMaster2"));
	JetEngineMaster2->SetupAttachment(JetMesh);
	JetEngineMaster2->SetRelativeScale3D(FVector(.1f , .1f , .2f));
	JetEngineMaster2->SetRelativeLocation(FVector(410 , 15 , 260));
	JetEngineMaster2->SetGenerateOverlapEvents(true);
	JetEngineMaster2->OnClicked.AddDynamic(this , &AL_Viper::OnMyEngineMaster2Clicked);
	JetEngineMaster2->SetHiddenInGame(false); // For Test

	JetJFSHandle = CreateDefaultSubobject<UBoxComponent>(TEXT("JetJFSHandle"));
	JetJFSHandle->SetupAttachment(JetMesh);
	JetJFSHandle->SetRelativeScale3D(FVector(.1f , .1f , .2f));
	JetJFSHandle->SetRelativeLocation(FVector(410 , 15 , 260));
	JetJFSHandle->SetGenerateOverlapEvents(true);
	JetJFSHandle->OnClicked.AddDynamic(this , &AL_Viper::OnMyJFSHandle1Clicked);
	JetJFSHandle->SetHiddenInGame(false); // For Test

	JetCanopy = CreateDefaultSubobject<UBoxComponent>(TEXT("JetCanopy"));
	JetCanopy->SetupAttachment(JetMesh);
	JetCanopy->SetRelativeScale3D(FVector(.05f , .2f , .05f));
	JetCanopy->SetRelativeLocation(CanopyNormalLoc);
	JetCanopy->SetGenerateOverlapEvents(true);
	JetCanopy->OnClicked.AddDynamic(this , &AL_Viper::OnMyCanopyClicked);
	JetCanopy->SetHiddenInGame(false); // For Test

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

	JetPostProcess = CreateDefaultSubobject<UPostProcessComponent>(TEXT("JetPostProcess"));
#pragma endregion

	CreateDumyComp();

	PushQueue();

	this->Tags = TArray<FName>();
	this->Tags.Add(FName("Viper"));

	bReplicates = true;
	SetReplicateMovement(true);
}
#pragma endregion

void AL_Viper::PushQueue()
{
	StartScenario.push("MIC");
	StartScenario.push("EngineGen");
	StartScenario.push("EngineControl");
	StartScenario.push("JFS_Switch");
	StartScenario.push("EngineMaster");
	StartScenario.push("JFS_Handle");
	StartScenario.push("Throttle");
	StartScenario.push("Canopy");
}

void AL_Viper::OnMyMeshOverlap(UPrimitiveComponent* OverlappedComponent , AActor* OtherActor ,
                               UPrimitiveComponent* OtherComp , int32 OtherBodyIndex , bool bFromSweep ,
                               const FHitResult& SweepResult)
{
	LOG_SCREEN("%s" , *OtherActor->GetName());
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
	else if(auto tp = Cast<AK_CesiumTeleportBox>(OtherActor))
	{
		JetTailVFXLeft->SetFloatParameter(FName("Lifetime") , 1.f);
		JetTailVFXRight->SetFloatParameter(FName("Lifetime") , 1.f);
	}
}

void AL_Viper::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AL_Viper , CurrentWeapon);
	DOREPLIFETIME(AL_Viper , FlareCurCnt);
	DOREPLIFETIME(AL_Viper , CanopyPitch);
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
	}
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
		if (StartScenario.size() > 0 && StartScenario.front().Equals("Throttle"))
		{
			StartScenario.pop();
			DummyThrottleMesh->SetRenderCustomDepth(false);
			DummyThrottleMesh->CustomDepthStencilValue = 0;
		}
	}
}

void AL_Viper::OnMyMicClicked(UPrimitiveComponent* TouchedComponent , FKey ButtonPressed)
{
	LOG_SCREEN("MIC 클릭");
	if (!bMIC)
	{
		bMIC = true;
		DummyMICMesh->SetRelativeRotation(FRotator(30 , 0 , 0));
		if (StartScenario.size() > 0 && StartScenario.front().Equals("MIC"))
		{
			StartScenario.pop();
			DummyMICMesh->SetRenderCustomDepth(false);
			DummyMICMesh->CustomDepthStencilValue = 0;
		}
	}
	else
	{
		bMIC = false;
		DummyMICMesh->SetRelativeRotation(FRotator(0 , 0 , 0));
	}
}

void AL_Viper::OnMyEngineGen1Clicked(UPrimitiveComponent* TouchedComponent , FKey ButtonPressed)
{
	LOG_SCREEN("EngineGen 클릭");
	if (!bEngineGen1)
	{
		bEngineGen1 = true;
		DummyEngineGenerMesh1->SetRelativeRotation(FRotator(30 , 0 , 0));
		if (StartScenario.size() > 0 && StartScenario.front().Equals("EngineGen"))
		{
			if (bEngineGen1 && bEngineGen2)
			{
				StartScenario.pop();
				DummyEngineGenerMesh1->SetRenderCustomDepth(false);
				DummyEngineGenerMesh1->CustomDepthStencilValue = 0;
				DummyEngineGenerMesh2->SetRenderCustomDepth(false);
				DummyEngineGenerMesh2->CustomDepthStencilValue = 0;
			}
		}
	}
	else
	{
		bEngineGen1 = false;
		DummyEngineGenerMesh1->SetRelativeRotation(FRotator(0 , 0 , 0));
	}
}

void AL_Viper::OnMyEngineGen2Clicked(UPrimitiveComponent* TouchedComponent , FKey ButtonPressed)
{
	LOG_SCREEN("EngineGen2 클릭");
	if (!bEngineGen2)
	{
		bEngineGen2 = true;
		DummyEngineGenerMesh2->SetRelativeRotation(FRotator(30 , 0 , 0));
		if (StartScenario.size() > 0 && StartScenario.front().Equals("EngineGen"))
		{
			if (bEngineGen1 && bEngineGen2)
			{
				StartScenario.pop();
				DummyEngineGenerMesh1->SetRenderCustomDepth(false);
				DummyEngineGenerMesh1->CustomDepthStencilValue = 0;
				DummyEngineGenerMesh2->SetRenderCustomDepth(false);
				DummyEngineGenerMesh2->CustomDepthStencilValue = 0;
			}
		}
	}
	else
	{
		bEngineGen2 = false;
		DummyEngineGenerMesh2->SetRelativeRotation(FRotator(0 , 0 , 0));
	}
}

void AL_Viper::OnMyEngineControlClicked(UPrimitiveComponent* TouchedComponent , FKey ButtonPressed)
{
	LOG_SCREEN("EngineControl 클릭");
	if (!bEngineControl1)
	{
		bEngineControl1 = true;
		DummyEngineControlMesh1->SetRelativeRotation(FRotator(30 , 0 , 0));
		if (StartScenario.size() > 0 && StartScenario.front().Equals("EngineControl"))
		{
			if (bEngineControl1 && bEngineControl2)
			{
				StartScenario.pop();
				DummyEngineControlMesh1->SetRenderCustomDepth(false);
				DummyEngineControlMesh1->CustomDepthStencilValue = 0;
				DummyEngineControlMesh2->SetRenderCustomDepth(false);
				DummyEngineControlMesh2->CustomDepthStencilValue = 0;
			}
		}
	}
	else
	{
		bEngineControl1 = false;
		DummyEngineControlMesh1->SetRelativeRotation(FRotator(0 , 0 , 0));
	}
}

void AL_Viper::OnMyEngineControl2Clicked(UPrimitiveComponent* TouchedComponent , FKey ButtonPressed)
{
	LOG_SCREEN("EngineControl2 클릭");
	if (!bEngineControl2)
	{
		bEngineControl2 = true;
		DummyEngineControlMesh2->SetRelativeRotation(FRotator(30 , 0 , 0));
		if (StartScenario.size() > 0 && StartScenario.front().Equals("EngineControl"))
		{
			if (bEngineControl1 && bEngineControl2)
			{
				StartScenario.pop();
				DummyEngineControlMesh1->SetRenderCustomDepth(false);
				DummyEngineControlMesh1->CustomDepthStencilValue = 0;
				DummyEngineControlMesh2->SetRenderCustomDepth(false);
				DummyEngineControlMesh2->CustomDepthStencilValue = 0;
			}
		}
	}
	else
	{
		bEngineControl2 = false;
		DummyEngineControlMesh2->SetRelativeRotation(FRotator(0 , 0 , 0));
	}
}

void AL_Viper::OnMyJetFuelStarterClicked(UPrimitiveComponent* TouchedComponent , FKey ButtonPressed)
{
	LOG_SCREEN("JFS 클릭");
	if (!bJFS)
	{
		bJFS = true;
		DummyJFSMesh->SetRelativeRotation(FRotator(30 , 0 , 0));
		if (StartScenario.size() > 0 && StartScenario.front().Equals("JFS_Switch"))
		{
			StartScenario.pop();
			DummyJFSMesh->SetRenderCustomDepth(false);
			DummyJFSMesh->CustomDepthStencilValue = 0;
		}
	}
	else
	{
		bJFS = false;
		DummyJFSMesh->SetRelativeRotation(FRotator(0 , 0 , 0));
	}
}

void AL_Viper::OnMyEngineMaster1Clicked(UPrimitiveComponent* TouchedComponent , FKey ButtonPressed)
{
	LOG_SCREEN("Engine Master1 클릭");
	if (!bEngineMaster1)
	{
		bEngineMaster1 = true;
		DummyEngineMasterMesh1->SetRelativeRotation(FRotator(30 , 0 , 0));
		if (StartScenario.size() > 0 && StartScenario.front().Equals("EngineMaster"))
		{
			if (bEngineMaster1 && bEngineMaster2)
			{
				StartScenario.pop();
				DummyEngineMasterMesh1->SetRenderCustomDepth(false);
				DummyEngineMasterMesh1->CustomDepthStencilValue = 0;
				DummyEngineMasterMesh2->SetRenderCustomDepth(false);
				DummyEngineMasterMesh2->CustomDepthStencilValue = 0;
			}
		}
	}
	else
	{
		bEngineMaster1 = false;
		DummyEngineMasterMesh1->SetRelativeRotation(FRotator(0 , 0 , 0));
	}
}

void AL_Viper::OnMyEngineMaster2Clicked(UPrimitiveComponent* TouchedComponent , FKey ButtonPressed)
{
	LOG_SCREEN("Engine Master2 클릭");
	if (!bEngineMaster2)
	{
		bEngineMaster2 = true;
		DummyEngineMasterMesh2->SetRelativeRotation(FRotator(30 , 0 , 0));
		if (StartScenario.size() > 0 && StartScenario.front().Equals("EngineMaster"))
		{
			if (bEngineMaster1 && bEngineMaster2)
			{
				StartScenario.pop();
				DummyEngineMasterMesh1->SetRenderCustomDepth(false);
				DummyEngineMasterMesh1->CustomDepthStencilValue = 0;
				DummyEngineMasterMesh2->SetRenderCustomDepth(false);
				DummyEngineMasterMesh2->CustomDepthStencilValue = 0;
			}
		}
	}
	else
	{
		bEngineMaster2 = false;
		DummyEngineMasterMesh2->SetRelativeRotation(FRotator(0 , 0 , 0));
	}
}

void AL_Viper::OnMyJFSHandle1Clicked(UPrimitiveComponent* TouchedComponent , struct FKey ButtonPressed)
{
	LOG_SCREEN("JFS 핸들 클릭");
	if (!bJFSHandle)
	{
		bJFSHandle = true;
		DummyJFSHandleMesh->SetRelativeRotation(FRotator(30 , 0 , 0));
		if (StartScenario.size() > 0 && StartScenario.front().Equals("JFS_Handle"))
		{
			StartScenario.pop();
			DummyJFSHandleMesh->SetRenderCustomDepth(false);
			DummyJFSHandleMesh->CustomDepthStencilValue = 0;
		}
	}
	else
	{
		bJFSHandle = false;
		DummyJFSHandleMesh->SetRelativeRotation(FRotator(0 , 0 , 0));
	}
}

void AL_Viper::OnMyCanopyClicked(UPrimitiveComponent* TouchedComponent , struct FKey ButtonPressed)
{
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
			newFpsYaw = UKismetMathLibrary::FClamp(newFpsYaw , -150.f , 150.f);
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
	if (JetPostProcess && JetPostProcess->Settings.WeightedBlendables.Array.Num() > 0)
		JetPostProcess->Settings.WeightedBlendables.Array[0].Weight = 0;

	if (!IsStart)
	{
		APlayerController* PlayerController = Cast<APlayerController>(GetWorld()->GetFirstLocalPlayerFromController());
		if (PlayerController)
		{
			PlayerController->SetIgnoreMoveInput(true);
			PlayerController->SetIgnoreLookInput(true);
		}
	}
}

void AL_Viper::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//PrintNetLog();

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

	if (IsFlyStart)
	{
		auto pc = Cast<APlayerController>(Controller);
		if (pc)
		{
			UEnhancedInputLocalPlayerSubsystem* subsys = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
				pc->GetLocalPlayer());
			if (subsys)
			{
				subsys->AddMappingContext(IMC_Fun , 0);
			}

			pc->bEnableClickEvents = true;
		}
		IsFlyStart = false;
	}

	// 시동 절차 단계
	if (!IsStart)
	{
		if (StartScenario.size() > 0)
		{
			FString ScenarioFront = StartScenario.front();
			UStaticMeshComponent* ScenarioComponent = nullptr;
			if (ScenarioFront.Equals("MIC"))
			{
				DummyMICMesh->SetRenderCustomDepth(true);
				DummyMICMesh->CustomDepthStencilValue = 1;
			}
			else if (ScenarioFront.Equals("EngineGen"))
			{
				DummyEngineGenerMesh1->SetRenderCustomDepth(true);
				DummyEngineGenerMesh1->CustomDepthStencilValue = 1;
				DummyEngineGenerMesh2->SetRenderCustomDepth(true);
				DummyEngineGenerMesh2->CustomDepthStencilValue = 1;
			}
			else if (ScenarioFront.Equals("EngineControl"))
			{
				DummyEngineControlMesh1->SetRenderCustomDepth(true);
				DummyEngineControlMesh1->CustomDepthStencilValue = 1;
				DummyEngineControlMesh2->SetRenderCustomDepth(true);
				DummyEngineControlMesh2->CustomDepthStencilValue = 1;
			}
			else if (ScenarioFront.Equals("JFS_Switch"))
			{
				DummyJFSMesh->SetRenderCustomDepth(true);
				DummyJFSMesh->CustomDepthStencilValue = 1;
			}
			else if (ScenarioFront.Equals("EngineMaster"))
			{
				DummyEngineMasterMesh1->SetRenderCustomDepth(true);
				DummyEngineMasterMesh1->CustomDepthStencilValue = 1;
				DummyEngineMasterMesh2->SetRenderCustomDepth(true);
				DummyEngineMasterMesh2->CustomDepthStencilValue = 1;
			}
			else if (ScenarioFront.Equals("JFS_Handle"))
			{
				DummyJFSHandleMesh->SetRenderCustomDepth(true);
				DummyJFSHandleMesh->CustomDepthStencilValue = 1;
			}
			else if (ScenarioFront.Equals("Throttle"))
			{
				DummyThrottleMesh->SetRenderCustomDepth(true);
				DummyThrottleMesh->CustomDepthStencilValue = 1;
			}
			else if (ScenarioFront.Equals("Canopy"))
			{
				DummyCanopyMesh->SetRenderCustomDepth(true);
				DummyCanopyMesh->CustomDepthStencilValue = 1;
				if (CanopyPitch == 0.f)
				{
					StartScenario.pop();
					DummyCanopyMesh->SetRenderCustomDepth(false);
					DummyCanopyMesh->CustomDepthStencilValue = 0;
				}
			}
		}
		else
		{
			if (JetPostProcess && JetPostProcess->Settings.WeightedBlendables.Array.Num() > 0)
				JetPostProcess->Settings.WeightedBlendables.Array[0].Weight = 0;
			IsStart = true;
			IsEngineOn = true;
		}
	}
	// 운행 단계
	else
	{
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

#pragma region Change Accel Value (backUp)
		// if (IsAccel)
		// {
		// 	KeyDownAccel += DeltaTime;
		// 	if (KeyDownAccel >= ChangeAccelTime)
		// 	{
		// 		KeyDownAccel = 0.f;
		//
		// 		// 기어 변경
		// 		AccelGear++;
		// 		if (AccelGear > 3)
		// 			AccelGear = 3;
		// 	}
		// }
		// else if (IsBreak)
		// {
		// 	KeyDownAccel += DeltaTime;
		// 	if (KeyDownAccel >= ChangeAccelTime)
		// 	{
		// 		KeyDownAccel = 0.f;
		//
		// 		// 기어 변경
		// 		AccelGear--;
		// 		if (AccelGear < 0)
		// 			AccelGear = 0;
		// 	}
		// }
#pragma endregion

#pragma region Change Accel Value2
		FVector engineLoc = JetFirstEngine->GetRelativeLocation();
		SetAccelGear();
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
			if (engineLoc.X > ThrottleMilLoc.X)
			{
				auto newEngineX = engineLoc.X - ThrottleMoveSpeed2;
				newEngineX = UKismetMathLibrary::FClamp(newEngineX , ThrottleMilLoc.X , ThrottleMaxLoc.X);
				if (ThrottleMaxLoc.X - newEngineX < 0.2)
					JetFirstEngine->SetRelativeLocation(ThrottleMilLoc);
				else
					JetFirstEngine->SetRelativeLocation(FVector(newEngineX , engineLoc.Y , engineLoc.Z));
			}
			else if (engineLoc.X > ThrottleOffLoc.X)
			{
				auto newEngineX = engineLoc.X - ThrottleMoveSpeed1;
				newEngineX = UKismetMathLibrary::FClamp(newEngineX , ThrottleOffLoc.X , ThrottleMilLoc.X);
				JetFirstEngine->SetRelativeLocation(FVector(newEngineX , engineLoc.Y , engineLoc.Z));
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

#pragma region LockOn
		IsLockOn();
#pragma endregion

#pragma region Flare Arrow Rotation Change
		if (CurrentWeapon == EWeapon::Flare)
		{
			int32 randRot = FMath::RandRange(-150 , -110);
			LOG_SCREEN("%d" , randRot);
			FRotator newFlareRot = FRotator(randRot , 0 , 0);
			JetFlareArrow1->SetRelativeRotation(newFlareRot);
			JetFlareArrow2->SetRelativeRotation(newFlareRot);
		}
#pragma endregion
	}

	if (IsLocallyControlled())
	{
		ChangeBooster();

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
			FRotator FPSrot = JetSprintArmFPS->GetRelativeRotation();
			//FRotator(-30 , 0 , 0)
			auto lerpFPSrot = FMath::Lerp(FPSrot , FRotator(-30 , 0 , 0) , DeltaTime);
			JetSprintArmFPS->SetRelativeRotation(lerpFPSrot);
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
	//MulticastRPCMissile(newOwner);
}

void AL_Viper::MulticastRPCMissile_Implementation(AActor* newOwner)
{
}

void AL_Viper::ServerRPCFlare_Implementation(AActor* newOwner)
{
	if (FlareFactory)
	{
		if (FlareCurCnt > 0)
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
				if (auto mai = Cast<IJ_MissionActorInterface>(hit.GetActor()))
				{
					MulticastRPCLockOn(hit.GetActor());
				}
				// if (hit.GetActor()->ActorHasTag("target"))
				// {
				// 	
				// }
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

void AL_Viper::CreateDumyComp()
{
	DummyMICMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DummyMICMesh"));
	DummyMICMesh->SetupAttachment(JetMic);
	DummyMICMesh->SetRelativeScale3D(FVector(.5f));

	DummyEngineGenerMesh1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DummyEngineGenMesh1"));
	DummyEngineGenerMesh1->SetupAttachment(JetEngineGen);
	DummyEngineGenerMesh1->SetRelativeScale3D(FVector(.5f));

	DummyEngineGenerMesh2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DummyEngineGenMesh2"));
	DummyEngineGenerMesh2->SetupAttachment(JetEngineGen2);
	DummyEngineGenerMesh2->SetRelativeScale3D(FVector(.5f));

	DummyEngineControlMesh1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DummyEngineControlMesh1"));
	DummyEngineControlMesh1->SetupAttachment(JetEngineControl);
	DummyEngineControlMesh1->SetRelativeScale3D(FVector(.5f));

	DummyEngineControlMesh2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DummyEngineControlMesh2"));
	DummyEngineControlMesh2->SetupAttachment(JetEngineControl2);
	DummyEngineControlMesh2->SetRelativeScale3D(FVector(.5f));

	DummyJFSMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DummyJFSMesh"));
	DummyJFSMesh->SetupAttachment(JetFuelStarter);
	DummyJFSMesh->SetRelativeScale3D(FVector(.5f));

	DummyEngineMasterMesh1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DummyEngineMasterMesh1"));
	DummyEngineMasterMesh1->SetupAttachment(JetEngineMaster);
	DummyEngineMasterMesh1->SetRelativeScale3D(FVector(.5f));

	DummyEngineMasterMesh2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DummyEngineMasterMesh2"));
	DummyEngineMasterMesh2->SetupAttachment(JetEngineMaster2);
	DummyEngineMasterMesh2->SetRelativeScale3D(FVector(.5f));

	DummyJFSHandleMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DummyJFSHandleMesh"));
	DummyJFSHandleMesh->SetupAttachment(JetJFSHandle);
	DummyJFSHandleMesh->SetRelativeScale3D(FVector(.5f));

	DummyThrottleMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DummyThrottleMesh"));
	DummyThrottleMesh->SetupAttachment(JetFirstEngine);
	DummyThrottleMesh->SetRelativeScale3D(FVector(.5f));

	DummyCanopyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DummyCanopyMesh"));
	DummyCanopyMesh->SetupAttachment(JetCanopy);
	DummyCanopyMesh->SetRelativeScale3D(FVector(.5f));
}

void AL_Viper::SetAccelGear()
{
	// 기어 변동 구간
	// 0%, 50%, 90%
	auto currAccelGear = JetFirstEngine->GetRelativeLocation().X;
	auto currValue = currAccelGear - ThrottleOffLoc.X;
	auto SizeValue = ThrottleMaxLoc.X - ThrottleOffLoc.X;
	auto per = currValue / SizeValue * 100;
	if (per <= 0)
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
