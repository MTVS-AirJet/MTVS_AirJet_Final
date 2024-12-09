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
#include "Components/Image.h"
#include "Components/PostProcessComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/TextBlock.h"
#include "Components/WidgetComponent.h"
#include "Engine/TextureRenderTarget2D.h"
#include "GameFramework/SpringArmComponent.h"
#include "JBS/J_MissionActorInterface.h"
#include "JBS/J_MissionPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "LHJ/L_Flare.h"
#include "LHJ/L_HUDWidget.h"
#include "LHJ/L_Missile.h"
#include "LHJ/L_RoadTrigger.h"
#include "Net/UnrealNetwork.h"
#include "KHS/K_GameInstance.h"
#include "KHS/K_GameState.h"
#include "KHS/K_StandbyWidget.h"
#include "GameFramework/PlayerState.h"
#include "JBS/J_CustomWidgetComponent.h"
#include "JBS/J_GroundTarget.h"
#include "JBS/J_MissionGamemode.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "LHJ/L_MissileCam.h"
#include "LHJ/L_PlayerNameWidget.h"
#include "LHJ/L_Target.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"

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

	JetLeftPannel = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("JetLeftPannel"));
	JetLeftPannel->SetRelativeLocation(FVector(-271 , -2 , 0));
	JetLeftPannel->SetRelativeScale3D(FVector(1.5 , 1 , 1));
	JetLeftPannel->SetupAttachment(JetMesh);
	JetLeftPannel->PrimaryComponentTick.bCanEverTick = false;
	JetLeftPannel->SetComponentTickEnabled(false);

	JetRightPannel = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("JetRightPannel"));
	JetRightPannel->SetRelativeLocation(FVector(-555 , 1 , 0));
	JetRightPannel->SetRelativeScale3D(FVector(2 , 1 , 1));
	JetRightPannel->SetupAttachment(JetMesh);
	JetRightPannel->PrimaryComponentTick.bCanEverTick = false;

	JetJFSPannel = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("JetJFSPannel"));
	JetJFSPannel->SetRelativeLocation(FVector(-137 , -6 , -17));
	JetJFSPannel->SetRelativeScale3D(FVector(1));
	JetJFSPannel->SetupAttachment(JetMesh);
	JetJFSPannel->PrimaryComponentTick.bCanEverTick = false;

	JetWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("JetWidget"));
	JetWidget->SetupAttachment(JetMesh);
	JetWidget->SetRelativeLocationAndRotation(FVector(420 , 0 , 295) , FRotator(0 , -180 , 0));
	JetWidget->SetDrawSize(FVector2D(200 , 150));

	MissileWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("MissileWidget"));
	MissileWidget->SetupAttachment(JetMesh);
	MissileWidget->SetRelativeLocationAndRotation(FVector(546.5 , 0.4 , 309.5) , FRotator(-14.5 , -180 , 0));
	MissileWidget->SetDrawSize(FVector2D(2048 , 2048));
	MissileWidget->SetRelativeScale3D(FVector(0.01));
	MissileWidget->SetVisibility(false);

	// 조종석 뷰
	JetSprintArmFPS = CreateDefaultSubobject<USpringArmComponent>(TEXT("JetSprintArmFPS"));
	JetSprintArmFPS->SetupAttachment(JetMesh);
	JetSprintArmFPS->SetRelativeLocationAndRotation(FVector(500 , 0 , 330) , FRotator(0 , 0 , 0));
	JetSprintArmFPS->TargetArmLength = 0.f;
	JetSprintArmFPS->bEnableCameraRotationLag = true;
	JetSprintArmFPS->CameraRotationLagSpeed = 3.5f;

	JetCameraFPS = CreateDefaultSubobject<UCameraComponent>(TEXT("JetCameraFPS"));
	JetCameraFPS->SetupAttachment(JetSprintArmFPS);
	JetCameraFPS->SetActive(false);

	JetSpringArmMissileCam = CreateDefaultSubobject<USpringArmComponent>(TEXT("JetSpringArmMissileCam"));
	JetSpringArmMissileCam->SetupAttachment(JetMesh);
	JetSpringArmMissileCam->SetRelativeRotation(FRotator(45 , 0 , 0));
	JetSpringArmMissileCam->bInheritPitch = false;
	JetSpringArmMissileCam->bInheritRoll = false;
	JetSpringArmMissileCam->bInheritYaw = false;

	JetCameraMissileCam = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("JetCameraMissileCam"));
	JetCameraMissileCam->SetupAttachment(JetSpringArmMissileCam);
	JetCameraMissileCam->SetRelativeRotation(FRotator(-25 , 0 , 0));
	JetCameraMissileCam->SetRelativeScale3D(FVector(.1 , 1 , .5));
	//JetCameraMissileCam->SetHiddenInGame(true);

	BoosterLeftVFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("BoosterLeftVFX"));
	BoosterLeftVFX->SetupAttachment(JetMesh);
	BoosterLeftVFX->SetRelativeLocationAndRotation(FVector(-780 , -50 , 180) , FRotator(0 , 180 , 0));
	ConstructorHelpers::FObjectFinder<UNiagaraSystem> LeftVFX(TEXT(
		"/Script/Niagara.NiagaraSystem'/Game/Asset/RocketThrusterExhaustFX/FX/NS_RocketExhaust_Red.NS_RocketExhaust_Red'"));
	if (LeftVFX.Succeeded())
	{
		BoosterLeftVFX->SetAsset(LeftVFX.Object);
		BoosterLeftVFX->SetVariableFloat(FName("EnergyCore_Life") , 0.f);
		BoosterLeftVFX->SetVariableFloat(FName("HeatHaze_Lifetime") , 0.f);
		BoosterLeftVFX->SetVariableFloat(FName("Particulate_Life") , 0.f);
		BoosterLeftVFX->SetVariableFloat(FName("Thrusters_Life") , 0.f);
	}

	BoosterRightVFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("BoosterRightVFX"));
	BoosterRightVFX->SetupAttachment(JetMesh);
	BoosterRightVFX->SetRelativeLocationAndRotation(FVector(-780 , 50 , 180) , FRotator(0 , 180 , 0));
	ConstructorHelpers::FObjectFinder<UNiagaraSystem> RightVFX(TEXT(
		"/Script/Niagara.NiagaraSystem'/Game/Asset/RocketThrusterExhaustFX/FX/NS_RocketExhaust_Red.NS_RocketExhaust_Red'"));
	if (RightVFX.Succeeded())
	{
		BoosterRightVFX->SetAsset(RightVFX.Object);
		BoosterRightVFX->SetVariableFloat(FName("EnergyCore_Life") , 0.f);
		BoosterRightVFX->SetVariableFloat(FName("HeatHaze_Lifetime") , 0.f);
		BoosterRightVFX->SetVariableFloat(FName("Particulate_Life") , 0.f);
		BoosterRightVFX->SetVariableFloat(FName("Thrusters_Life") , 0.f);
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

	AirResistanceVFX = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AirResistanceVFX"));
	AirResistanceVFX->SetupAttachment(JetMesh);
	AirResistanceVFX->SetRelativeLocation(FVector(-150 , -16.5 , 285));
	ConstructorHelpers::FObjectFinder<UStaticMesh> AirResistanceMesh(TEXT(
		"/Script/Engine.StaticMesh'/Game/Blueprints/LHJ/resource/Mat/wings_snoke.wings_snoke'"));
	if (AirResistanceMesh.Succeeded())
	{
		AirResistanceVFX->SetStaticMesh(AirResistanceMesh.Object);

		ConstructorHelpers::FObjectFinder<UMaterial> AirVFX(TEXT(
			"/Script/Engine.Material'/Game/Asset/TrailPack/Materials/M_Wave.M_Wave'"));
		if (AirVFX.Succeeded())
		{
			AirResistanceVFX->SetMaterial(0 , AirVFX.Object);
		}
	}

	JetAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("JetAudio"));
	JetAudio->SetupAttachment(RootComponent);
	JetAudio->SetRelativeLocation(FVector(0 , 0 , 245));

	JetCanopyAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("JetCanopyAudio"));
	JetCanopyAudio->SetupAttachment(RootComponent);

	JetMICAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("JetMICAudio"));
	JetMICAudio->SetupAttachment(RootComponent);
	JetMICAudio->SetRelativeLocation(FVector(0 , 0 , 245));

	JetEngineAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("JetEngineAudio"));
	JetEngineAudio->SetupAttachment(RootComponent);
	JetEngineAudio->SetRelativeLocation(FVector(0 , 0 , 245));

	JetAirAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("JetAirAudio"));
	JetAirAudio->SetupAttachment(RootComponent);
	JetAirAudio->SetRelativeLocation(FVector(0 , 0 , 245));

	JetMissileAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("JetMissileAudio"));
	JetMissileAudio->SetupAttachment(RootComponent);
	JetMissileAudio->SetRelativeLocation(FVector(0 , 0 , 50));

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

	PlayerNameSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("PlayerNameSpringArm"));
	PlayerNameSpringArm->SetupAttachment(RootComponent);
	PlayerNameSpringArm->SetRelativeRotation(FRotator(-90 , 0 , 0));
	PlayerNameSpringArm->bInheritPitch = false;
	PlayerNameSpringArm->bInheritRoll = false;
	PlayerNameSpringArm->bInheritYaw = false;

	PlayerNameWidgetComponent = CreateDefaultSubobject<UJ_CustomWidgetComponent>(TEXT("PlayerNameWidgetComponent"));
	PlayerNameWidgetComponent->SetupAttachment(PlayerNameSpringArm);
	PlayerNameWidgetComponent->SetIsReplicated(true);
	PlayerNameWidgetComponent->SetCollisionProfileName(FName(TEXT("NoCollision")));
#pragma endregion

#pragma region Prop2
	JetPropRootScene = CreateDefaultSubobject<USceneComponent>(TEXT("JetPropRootScene"));
	JetPropRootScene->SetupAttachment(JetMesh);

	JetPropObj4 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("JetPropObj4"));
	JetPropObj4->SetupAttachment(JetPropRootScene);
	JetPropObj4->SetRelativeLocationAndRotation(FVector(557 , 0 , 290) , FRotator(-10 , 0 , 0));
	JetPropObj11 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("JetPropObj11"));
	JetPropObj11->SetupAttachment(JetPropRootScene);
	JetPropObj11->SetRelativeLocationAndRotation(FVector(555.5 , -37.5 , 296) , FRotator(-10 , 0 , 0));
	JetPropObj9 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("JetPropObj9"));
	JetPropObj9->SetupAttachment(JetPropRootScene);
	JetPropObj9->SetRelativeLocationAndRotation(FVector(555.5 , 37.4 , 298) , FRotator(-10 , 0 , 0));
	JetPropObj8 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("JetPropObj8"));
	JetPropObj8->SetupAttachment(JetPropRootScene);
	JetPropObj8->SetRelativeLocationAndRotation(FVector(555.5 , 24 , 276) , FRotator(-10 , 0 , 0));
	JetPropObj8->SetRelativeScale3D(FVector(1 , 1.1 , 1.3));
	JetPropObj7 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("JetPropObj7"));
	JetPropObj7->SetupAttachment(JetPropRootScene);
	JetPropObj7->SetRelativeLocationAndRotation(FVector(555.5 , -23.5 , 276) , FRotator(-10 , 0 , 0));
	JetPropObj7->SetRelativeScale3D(FVector(1 , 1.3 , 1.5));

	JetProp0_C = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("JetProp0_C"));
	JetProp0_C->SetupAttachment(JetMesh , FName("Prop0_C"));
	JetProp2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("JetProp2"));
	JetProp2->SetupAttachment(JetMesh , FName("Prop2"));
	JetProp1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("JetProp1"));
	JetProp1->SetupAttachment(JetMesh , FName("Prop1"));
	JetProp3_L = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("JetProp3_L"));
	JetProp3_L->SetupAttachment(JetMesh , FName("Prop3_L"));
	JetProp0_L = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("JetProp0_L"));
	JetProp0_L->SetupAttachment(JetMesh , FName("Prop0_L"));
	JetProp10 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("JetProp10"));
	JetProp10->SetupAttachment(JetMesh , FName("Prop10"));
	JetProp0_R = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("JetProp0_R"));
	JetProp0_R->SetupAttachment(JetMesh , FName("Prop0_R"));
	JetProp3_R = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("JetProp3_R"));
	JetProp3_R->SetupAttachment(JetMesh , FName("Prop3_R"));
	JetProp6_L = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("JetProp6_L"));
	JetProp6_L->SetupAttachment(JetMesh , FName("Prop6_L"));
	JetProp6_R = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("JetProp6_R"));
	JetProp6_R->SetupAttachment(JetMesh , FName("Prop6_R"));
#pragma endregion

	CreateDumyComp();

	PushQueue();

	this->Tags = TArray<FName>();
	this->Tags.Add(FName("Viper"));

	// 초기 회전값 설정
	QuatCurrentRotation = FQuat::Identity;
	QuatTargetRotation = FQuat::Identity;

	bReplicates = true;
	SetReplicateMovement(false);
}
#pragma endregion

#pragma region Create Prop Component
void AL_Viper::CreateDumyComp()
{
#pragma region MIC
	DummyMICMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DummyMICMesh"));
	DummyMICMesh->SetRelativeLocationAndRotation(FVector(503.5 , -45 , 250) , FRotator(30 , 0 , 0));
	DummyMICMesh->SetupAttachment(JetMesh);
	DummyMICMesh->SetRelativeScale3D(FVector(3));

	JetMic = CreateDefaultSubobject<UBoxComponent>(TEXT("JetMic"));
	JetMic->SetupAttachment(DummyMICMesh);
	JetMic->SetRelativeLocation(FVector(0 , 0 , .8));
	JetMic->SetBoxExtent(FVector(.2 , .15 , .8));
	JetMic->SetGenerateOverlapEvents(true);
	JetMic->OnClicked.AddDynamic(this , &AL_Viper::OnMyMicClicked);
#pragma endregion

#pragma region Engine Generater
	DummyEngineGenerMesh1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DummyEngineGenMesh1"));
	DummyEngineGenerMesh1->SetupAttachment(JetMesh);
	DummyEngineGenerMesh1->SetRelativeLocationAndRotation(FVector(524 , 33 , 250) , FRotator(30 , 0 , 0));
	DummyEngineGenerMesh1->SetRelativeScale3D(FVector(3));

	JetEngineGen = CreateDefaultSubobject<UBoxComponent>(TEXT("JetEngineGen"));
	JetEngineGen->SetupAttachment(DummyEngineGenerMesh1);
	JetEngineGen->SetBoxExtent(FVector(.2 , .15 , .8));
	JetEngineGen->SetRelativeLocation(FVector(0 , 0 , .8));
	JetEngineGen->SetGenerateOverlapEvents(true);
	JetEngineGen->OnClicked.AddDynamic(this , &AL_Viper::OnMyEngineGen1Clicked);

	DummyEngineGenerMesh2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DummyEngineGenMesh2"));
	DummyEngineGenerMesh2->SetupAttachment(JetMesh);
	DummyEngineGenerMesh2->SetRelativeScale3D(FVector(3));
	DummyEngineGenerMesh2->SetRelativeLocationAndRotation(FVector(524 , 34 , 250) , FRotator(30 , 0 , 0));

	JetEngineGen2 = CreateDefaultSubobject<UBoxComponent>(TEXT("JetEngineGen2"));
	JetEngineGen2->SetupAttachment(DummyEngineGenerMesh2);
	JetEngineGen2->SetBoxExtent(FVector(.2 , .15 , .8));
	JetEngineGen2->SetRelativeLocation(FVector(0 , 0 , .8));
	JetEngineGen2->SetGenerateOverlapEvents(true);
	JetEngineGen2->OnClicked.AddDynamic(this , &AL_Viper::OnMyEngineGen2Clicked);
#pragma endregion

#pragma region Engine Control
	DummyEngineControlMesh1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DummyEngineControlMesh1"));
	DummyEngineControlMesh1->SetupAttachment(JetMesh);
	DummyEngineControlMesh1->SetRelativeLocationAndRotation(FVector(524 , 37.5 , 250) , FRotator(30 , 0 , 0));
	DummyEngineControlMesh1->SetRelativeScale3D(FVector(3));

	JetEngineControl = CreateDefaultSubobject<UBoxComponent>(TEXT("JetEngineControl"));
	JetEngineControl->SetupAttachment(DummyEngineControlMesh1);
	JetEngineControl->SetBoxExtent(FVector(.2 , .15 , .8));
	JetEngineControl->SetRelativeLocation(FVector(0 , 0 , .8));
	JetEngineControl->SetGenerateOverlapEvents(true);
	JetEngineControl->OnClicked.AddDynamic(this , &AL_Viper::OnMyEngineControlClicked);

	DummyEngineControlMesh2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DummyEngineControlMesh2"));
	DummyEngineControlMesh2->SetupAttachment(JetMesh);
	DummyEngineControlMesh2->SetRelativeLocationAndRotation(FVector(524 , 38.5 , 250) , FRotator(30 , 0 , 0));
	DummyEngineControlMesh2->SetRelativeScale3D(FVector(3));

	JetEngineControl2 = CreateDefaultSubobject<UBoxComponent>(TEXT("JetEngineControl2"));
	JetEngineControl2->SetupAttachment(DummyEngineControlMesh2);
	JetEngineControl2->SetBoxExtent(FVector(.2 , .15 , .8));
	JetEngineControl2->SetRelativeLocation(FVector(0 , 0 , .8));
	JetEngineControl2->SetGenerateOverlapEvents(true);
	JetEngineControl2->OnClicked.AddDynamic(this , &AL_Viper::OnMyEngineControl2Clicked);
#pragma endregion

#pragma region JFS
	DummyJFSMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DummyJFSMesh"));
	DummyJFSMesh->SetupAttachment(JetMesh);
	DummyJFSMesh->SetRelativeLocationAndRotation(FVector(518 , 36.5 , 250) , FRotator(30 , 0 , 0));
	DummyJFSMesh->SetRelativeScale3D(FVector(3));

	JetFuelStarter = CreateDefaultSubobject<UBoxComponent>(TEXT("JetFuelStarter"));
	JetFuelStarter->SetupAttachment(DummyJFSMesh);
	JetFuelStarter->SetBoxExtent(FVector(.2 , .15 , .8));
	JetFuelStarter->SetRelativeLocation(FVector(0 , 0 , .8));
	JetFuelStarter->SetGenerateOverlapEvents(true);
	JetFuelStarter->OnClicked.AddDynamic(this , &AL_Viper::OnMyJetFuelStarterClicked);
#pragma endregion

#pragma region Engine Master
	DummyEngineMasterMesh1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DummyEngineMasterMesh1"));
	DummyEngineMasterMesh1->SetupAttachment(JetMesh);
	DummyEngineMasterMesh1->SetRelativeLocationAndRotation(FVector(518 , 33.5 , 250) , FRotator(30 , 0 , 0));
	DummyEngineMasterMesh1->SetRelativeScale3D(FVector(3));

	JetEngineMaster = CreateDefaultSubobject<UBoxComponent>(TEXT("JetEngineMaster"));
	JetEngineMaster->SetupAttachment(DummyEngineMasterMesh1);
	JetEngineMaster->SetBoxExtent(FVector(.2 , .15 , .8));
	JetEngineMaster->SetRelativeLocation(FVector(0 , 0 , .8));
	JetEngineMaster->SetGenerateOverlapEvents(true);
	JetEngineMaster->OnClicked.AddDynamic(this , &AL_Viper::OnMyEngineMaster1Clicked);

	DummyEngineMasterMesh2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DummyEngineMasterMesh2"));
	DummyEngineMasterMesh2->SetupAttachment(JetMesh);
	DummyEngineMasterMesh2->SetRelativeLocationAndRotation(FVector(518 , 38 , 250) , FRotator(30 , 0 , 0));
	DummyEngineMasterMesh2->SetRelativeScale3D(FVector(3));

	JetEngineMaster2 = CreateDefaultSubobject<UBoxComponent>(TEXT("JetEngineMaster2"));
	JetEngineMaster2->SetupAttachment(DummyEngineMasterMesh2);
	JetEngineMaster2->SetBoxExtent(FVector(.2 , .15 , .8));
	JetEngineMaster2->SetRelativeLocation(FVector(0 , 0 , .8));
	JetEngineMaster2->SetGenerateOverlapEvents(true);
	JetEngineMaster2->OnClicked.AddDynamic(this , &AL_Viper::OnMyEngineMaster2Clicked);
#pragma endregion

#pragma region JFS Handle
	DummyJFSHandleMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DummyJFSHandleMesh"));
	DummyJFSHandleMesh->SetupAttachment(JetMesh);
	DummyJFSHandleMesh->SetRelativeLocation(FVector(-1100 , -63.5 , -502.5));
	DummyJFSHandleMesh->SetRelativeScale3D(FVector(3));

	JetJFSHandle = CreateDefaultSubobject<UBoxComponent>(TEXT("JetJFSHandle"));
	JetJFSHandle->SetupAttachment(DummyJFSHandleMesh);
	JetJFSHandle->SetRelativeLocation(FVector(548.5 , 32.7 , 253.5));
	JetJFSHandle->SetRelativeScale3D(FVector(.6 , .3 , .6));
	JetJFSHandle->SetBoxExtent(FVector(3 , 2 , 2));
	JetJFSHandle->SetGenerateOverlapEvents(true);
	JetJFSHandle->OnClicked.AddDynamic(this , &AL_Viper::OnMyJFSHandle1Clicked);
#pragma endregion

#pragma region Throttle
	DummyThrottleMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DummyThrottleMesh"));
	DummyThrottleMesh->SetupAttachment(JetMesh);
	DummyThrottleMesh->SetRelativeScale3D(FVector(1.5 , 1.5 , 1.5));
	DummyThrottleMesh->SetRelativeLocation(FVector(515 , -35 , 250));

	JetFirstEngine = CreateDefaultSubobject<UBoxComponent>(TEXT("JetFirstEngine"));
	JetFirstEngine->SetupAttachment(DummyThrottleMesh);
	JetFirstEngine->SetRelativeLocation(FVector(0 , 0 , 6.5));
	JetFirstEngine->SetBoxExtent(FVector(3 , 3 , 5));
	JetFirstEngine->SetRelativeScale3D(FVector(.8 , 1 , 1.3));
	JetFirstEngine->SetGenerateOverlapEvents(true);
	JetFirstEngine->OnClicked.AddDynamic(this , &AL_Viper::OnMyFirstEngineClicked);
#pragma endregion

#pragma region Canopy
	DummyCanopyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DummyCanopyMesh"));
	DummyCanopyMesh->SetupAttachment(JetMesh);
	DummyCanopyMesh->SetRelativeLocation(CanopyNormalLoc);

	JetCanopy = CreateDefaultSubobject<UBoxComponent>(TEXT("JetCanopy"));
	JetCanopy->SetupAttachment(DummyCanopyMesh);
	JetCanopy->SetBoxExtent(FVector(3 , 6 , 3));
	JetCanopy->SetRelativeLocation(FVector(522 , 42 , 269));
	JetCanopy->SetGenerateOverlapEvents(true);
	JetCanopy->OnClicked.AddDynamic(this , &AL_Viper::OnMyCanopyClicked);
#pragma endregion

#pragma region Break Hold
	DummyJFSBreakHold = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DummyJFSBreakHold"));
	DummyJFSBreakHold->SetupAttachment(JetMesh);
	DummyJFSBreakHold->SetRelativeLocationAndRotation(FVector(551 , 42 , 259.5) , FRotator(120 , 0 , 0));
	DummyJFSBreakHold->SetRelativeScale3D(FVector(3));

	JetBreakHold = CreateDefaultSubobject<UBoxComponent>(TEXT("JetBreakHold"));
	JetBreakHold->SetupAttachment(DummyJFSBreakHold);
	JetBreakHold->SetRelativeLocation(FVector(0 , 0 , .8));
	JetBreakHold->SetBoxExtent(FVector(.2 , .15 , .8));
	JetBreakHold->SetGenerateOverlapEvents(true);
	JetBreakHold->OnClicked.AddDynamic(this , &AL_Viper::OnMyBreakHoldClicked);
#pragma endregion

	DummyStick = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("DummyStick"));
	DummyStick->SetupAttachment(JetMesh);
	DummyStick->SetRelativeLocation(FVector(539 , .3 , 227.5));
	DummyStick->SetRelativeScale3D(FVector(1.2 , 1.2 , 1.4));

	JetRotationStick = CreateDefaultSubobject<UBoxComponent>(TEXT("JetRotationStick"));
	JetRotationStick->SetupAttachment(DummyStick);
	JetRotationStick->SetBoxExtent(FVector(1.5 , 1.5 , 5));
	JetRotationStick->SetRelativeLocation(FVector(-1.9 , 0 , 28.5));
	JetRotationStick->SetGenerateOverlapEvents(true);

	RotationStickConstraint = CreateDefaultSubobject<UPhysicsConstraintComponent>(TEXT("RotationStickConstraint"));
	RotationStickConstraint->SetupAttachment(DummyStick);
	RotationStickConstraint->SetAngularSwing1Limit(ACM_Limited , 20.f);
	RotationStickConstraint->SetAngularSwing2Limit(ACM_Limited , 20.f);
	RotationStickConstraint->SetAngularTwistLimit(ACM_Locked , 45.f);
}
#pragma endregion

#pragma region 시동절차 순서 정의
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
#pragma endregion

void AL_Viper::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AL_Viper , CurrentWeapon);
	DOREPLIFETIME(AL_Viper , FlareCurCnt);
	DOREPLIFETIME(AL_Viper , CanopyPitch);
	DOREPLIFETIME(AL_Viper , FrontWheel);
	DOREPLIFETIME(AL_Viper , RearWheel);
	DOREPLIFETIME(AL_Viper , IsEngineOn);
	DOREPLIFETIME(AL_Viper , bOnceUpdatePlayerName);
	// DOREPLIFETIME(AL_Viper , QuatCurrentRotation);
	// DOREPLIFETIME(AL_Viper , QuatTargetRotation);
}

void AL_Viper::OnMyMeshOverlap(UPrimitiveComponent* OverlappedComponent , AActor* OtherActor ,
                               UPrimitiveComponent* OtherComp , int32 OtherBodyIndex , bool bFromSweep ,
                               const FHitResult& SweepResult)
{
	if (auto RT = Cast<AL_RoadTrigger>(OtherActor))
	{
		if (RT->TriggerIdx == 0)
		{
			intTriggerNum = 1;
			if (IsLocallyControlled())
			{
				if (auto pc = Cast<AJ_MissionPlayerController>(Controller))
				{
					pc->CRPC_SetMissionTextUI(20);
					pc->CRPC_PlayCommanderVoice3(20);
				}
			}
		}
		else if (RT->TriggerIdx == 1)
		{
			intTriggerNum = 2;
			IsFlyStart = true;
			if (IsLocallyControlled())
			{
				if (auto pc = Cast<AJ_MissionPlayerController>(Controller))
				{
					pc->CRPC_SetMissionTextUI(21);
					pc->CRPC_PlayCommanderVoice3(21);
				}
			}
			CRPC_AudioControl(false);
		}
	}
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
		input->BindAction(IA_ViperLook , ETriggerEvent::Triggered , this , &AL_Viper::F_ViperLook);

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
		DummyThrottleMesh->SetRelativeLocation(FVector(ThrottleOffLoc.X + per ,
		                                               DummyThrottleMesh->GetRelativeLocation().Y ,
		                                               DummyThrottleMesh->GetRelativeLocation().Z));
		AccelGear = 1;
		bFirstEngine = true;
	}
}

void AL_Viper::OnMyMicClicked(UPrimitiveComponent* TouchedComponent , FKey ButtonPressed)
{
	CRPC_PlaySwitchSound(TouchedComponent->GetComponentLocation());
	if (!bMIC)
	{
		bMIC = true;
		DummyMICMesh->SetRelativeRotation(FRotator(-30 , 0 , 0));
		CRPC_MICAudioControl(true);
	}
	else
	{
		bMIC = false;
		DummyMICMesh->SetRelativeRotation(FRotator(30 , 0 , 0));
		CRPC_MICAudioControl(false);
	}
}

void AL_Viper::OnMyEngineGen1Clicked(UPrimitiveComponent* TouchedComponent , FKey ButtonPressed)
{
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
		CRPC_EngineSound(true , 0);
	}
	else
	{
		//bJFSHandle = false;
		DummyJFSHandleMesh->AddRelativeLocation(FVector(-1 , 0 , 0));
		FTimerHandle timerHandle;
		GetWorld()->GetTimerManager().SetTimer(timerHandle , [&]()
		{
			DummyJFSHandleMesh->AddRelativeLocation(FVector(1 , 0 , 0));
		} , 1.f , false);
	}
}

void AL_Viper::OnMyCanopyClicked(UPrimitiveComponent* TouchedComponent , struct FKey ButtonPressed)
{
	CRPC_PlaySwitchSound(TouchedComponent->GetComponentLocation());
	auto currLoc = DummyCanopyMesh->GetRelativeLocation();

	if (FVector::Dist(currLoc , CanopyCloseLoc) <= 1)
	{
		DummyCanopyMesh->SetRelativeLocation(CanopyHoldLoc);
		CRPC_CanopyAudioControl(true , 1);
		iCanopyNum = 3;
	}
	else if (FVector::Dist(currLoc , CanopyNormalLoc) <= 1)
	{
		DummyCanopyMesh->SetRelativeLocation(CanopyCloseLoc);
		CRPC_CanopyAudioControl(true , 0);
		iCanopyNum = 2;
	}
	else if (FVector::Dist(currLoc , CanopyOpenLoc) <= 1)
	{
		DummyCanopyMesh->SetRelativeLocation(CanopyNormalLoc);
		CRPC_CanopyAudioControl(false);
		iCanopyNum = 1;
	}
}

void AL_Viper::OnMyBreakHoldClicked(UPrimitiveComponent* TouchedComponent , struct FKey ButtonPressed)
{
	CRPC_PlaySwitchSound(TouchedComponent->GetComponentLocation());
	if (!bBreakHold)
	{
		if (!IsFirstBreakHoldClick)
		{
			if (auto pc = Cast<AJ_MissionPlayerController>(Controller))
			{
				pc->CRPC_SetMissionTextUI(17);
				pc->CRPC_PlayCommanderVoice3(17);
			}
			IsFirstBreakHoldClick = true;
			bStartAudio = true;
		}

		bBreakHold = true;
		DummyJFSBreakHold->SetRelativeRotation(FRotator(90 , 0 , 0));
	}
	else
	{
		bBreakHold = false;
		DummyJFSBreakHold->SetRelativeRotation(FRotator(120 , 0 , 0));
	}
}

void AL_Viper::F_ViperLook(const FInputActionValue& value)
{
	auto v = value.Get<FVector2D>();
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

void AL_Viper::F_ViperResetRotation(const FInputActionValue& value)
{
	SetActorRotation(FRotator(0 , JetMesh->GetComponentRotation().Yaw , 0));
	JetArrow->SetRelativeRotation(FRotator(0 , JetArrow->GetRelativeRotation().Yaw , 0));
}

void AL_Viper::F_ViperAccelStarted(const FInputActionValue& value)
{
	if (!bThrottleBreak)
		bThrottleAccel = true;
}

void AL_Viper::F_ViperAccelCompleted(const FInputActionValue& value)
{
	bThrottleAccel = false;
}

void AL_Viper::F_ViperBreakStarted(const FInputActionValue& value)
{
	if (!bThrottleAccel)
		bThrottleBreak = true;
}

void AL_Viper::F_ViperBreakCompleted(const FInputActionValue& value)
{
	bThrottleBreak = false;
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
	{
		JetCamera->SetActive(false);
		JetSprintArm->PrimaryComponentTick.bCanEverTick = false;
		JetCamera->PrimaryComponentTick.bCanEverTick = false;
	}
	if (JetCameraFPS)
	{
		if (!bStartMission/* && StartMissionViper_Del.IsBound()*/)
		{
			//StartMissionViper_Del.Broadcast();
			bStartMission = true;

			if (IsLocallyControlled())
			{
				if (auto pc = Cast<AJ_MissionPlayerController>(Controller))
				{
					pc->CRPC_SetMissionTextUI(6);
					pc->CRPC_PlayCommanderVoice3(6);
				}
				CRPC_AudioControl(true , 0);
			}
		}

		if (JetPostProcess && JetPostProcess->Settings.WeightedBlendables.Array.Num() > 0)
			JetPostProcess->Settings.WeightedBlendables.Array[0].Weight = 1;
		JetCameraFPS->SetActive(true);
		JetSprintArmFPS->PrimaryComponentTick.bCanEverTick = true;
		JetCameraFPS->PrimaryComponentTick.bCanEverTick = true;

		if (JetAudio)
		{
			if (auto attnuation = JetAudio->AttenuationSettings.Get())
			{
				if (attnuation->Attenuation.bAttenuate)
					attnuation->Attenuation.bAttenuate = false;
			}
		}
	}
}

void AL_Viper::F_ViperTpsStarted(const struct FInputActionValue& value)
{
	if (JetCamera)
	{
		JetCamera->SetActive(true);
		JetSprintArm->PrimaryComponentTick.bCanEverTick = true;
		JetCamera->PrimaryComponentTick.bCanEverTick = true;
	}

	if (JetCameraFPS)
	{
		if (JetPostProcess && JetPostProcess->Settings.WeightedBlendables.Array.Num() > 0)
			JetPostProcess->Settings.WeightedBlendables.Array[0].Weight = 0;
		JetCameraFPS->SetActive(false);
		JetSprintArmFPS->PrimaryComponentTick.bCanEverTick = false;
		JetCameraFPS->PrimaryComponentTick.bCanEverTick = false;
		if (JetAudio)
		{
			if (auto attnuation = JetAudio->AttenuationSettings.Get())
			{
				if (attnuation->Attenuation.bAttenuate)
					attnuation->Attenuation.bAttenuate = true;
			}
		}
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
				if (DummyCanopyMesh && iCanopyNum != 2)
				{
					DummyCanopyMesh->SetRelativeLocation(CanopyCloseLoc);
					iCanopyNum = 2;
				}

				break;
			}
		}
	}
#pragma endregion
}

void AL_Viper::F_ViperMoveTrigger(const struct FInputActionValue& value)
{
	FVector2D moveVector = value.Get<FVector2D>();
	moveVector = moveVector / 4 * 3;
	// 입력값에 최대 회전 각도 제한 적용
	float RollAngle = 0.f;
	float PitchAngle = 0.f;

	if ((moveVector.Y > 0.6f || moveVector.Y < -0.6f) && (moveVector.X > 0.6f || moveVector.X < -0.6f))
	{
		RollAngle = moveVector.Y * MaxRotationAngle / 9.f;
		PitchAngle = moveVector.X * MaxRotationAngle / 8.f;
	}
	else
	{
		RollAngle = moveVector.Y * MaxRotationAngle / 5.f;
		PitchAngle = moveVector.X * MaxRotationAngle / 10.f;
	}

	// Roll과 Pitch를 쿼터니언 회전으로 변환
	FQuat RollRotation = FQuat(FVector(1 , 0 , 0) , FMath::DegreesToRadians(RollAngle));
	FQuat PitchRotation = FQuat(FVector(0 , 1 , 0) , FMath::DegreesToRadians(PitchAngle));

	// 목표 회전 설정 (RootComponent를 기준으로)
	QuatTargetRotation = QuatCurrentRotation * RollRotation * PitchRotation;
	RollAngle = 0.f;
	PitchAngle = 0.f;

#pragma region Retate Pawn
	QuatCurrentRotation = FQuat::Slerp(QuatCurrentRotation , QuatTargetRotation ,
	                                   RotationSpeed * GetWorld()->GetDeltaSeconds());

	// 서버일때 회전을 하면 SRPC에서 한번 더 회전되기 때문에 문제될 수 있다. 
	if (!HasAuthority())
		SetActorRotation(QuatCurrentRotation);

	//SetActorRotation(QuatTargetRotation);
	ServerRPCRotation(QuatCurrentRotation);
	//ServerRPCRotation(QuatTargetRotation);

	if (bJetAirVFXOn)
	{
		if (GetActorRotation().Pitch > 10 && QuatCurrentRotation.Rotator().Pitch <= QuatTargetRotation.Rotator().Pitch)
			CRPC_AirSound(true);
		else
			CRPC_AirSound(false);
	}
#pragma endregion
}

void AL_Viper::F_ViperMoveCompleted(const struct FInputActionValue& value)
{
	//QuatTargetRotation = QuatCurrentRotation;
}
#pragma endregion

void AL_Viper::BeginPlay()
{
	Super::BeginPlay();

	JetCameraFPS->SetActive(false);

	CRPC_CanopyAudioControl(false);
	CRPC_AudioControl(false);
	CRPC_AirSound(false);

	if (auto gm = Cast<AJ_MissionGamemode>(GetWorld()->GetAuthGameMode()))
	{
		gm->missionEndDel.AddDynamic(this , &AL_Viper::StopAllVoice);
	}

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
			//LOG_S(Warning , TEXT("GameInstance doesn't exist"));
		}
		//GI에서 자신의 로그인 ID를 받아오고
		FString MyUserID = KGameInstace->GetUserId();

		auto kpc = Cast<AK_PlayerController>(GetOwner());
		//클라이언트일때
		if (kpc && kpc->IsLocalController())
		{
			//LOG_S(Warning , TEXT("MyUserID : %s") , *MyUserID);
			//ServerRPC함수를 호출
			ServerRPC_SetConnectedPlayerNames(MyUserID);
		}
	}
	AirResistanceVFX->SetVisibility(false);

	QuatCurrentRotation = GetActorRotation().Quaternion();

	JetCameraMissileCam->HideActorComponents(this);

	if (IsLocallyControlled())
	{
		GetWorld()->GetTimerManager().SetTimer(syncLocTimer , [this]()
		{
			ServerRPC_SyncLocation(this->GetActorLocation());
		} , .01f , true);
	}
}

void AL_Viper::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// PrintNetLog();

	if (bOnceUpdatePlayerName)
	{
		LOG_S(Warning , TEXT("%s bOnceUpdatePlayerName is true") , *this->GetName());
		ServerRPCRotation(GetActorRotation().Quaternion());
		// visible = true
		// SRPC_VisiblePlayerName();
		//
		// if (IsLocallyControlled())
		// {
		// 	PlayerNameWidgetComponent->SetHiddenInGame(true);
		// 	PlayerNameWidgetComponent->SetVisibility(false);
		// }
		if (!IsLocallyControlled())
		{
			PlayerNameWidgetComponent->SetHiddenInGame(false);
			PlayerNameWidgetComponent->SetVisibility(true);			
		}

		// target setting
		if (auto p = UGameplayStatics::GetPlayerPawn(GetWorld() , 0))
			PlayerNameWidgetComponent->SetTargetActor(p);

		// 이름 설정
		if (IsLocallyControlled())
		{
			if (auto gi = Cast<UK_GameInstance>(GetGameInstance()))
			{
				SRPC_SetMyName(gi->GetUserId());
			}
		}

		bOnceUpdatePlayerName = !bOnceUpdatePlayerName;
	}

#pragma region 제트엔진 이펙트
	if (bJetTailVFXOn)
	{
		if (JetTailVFXLeft && JetTailVFXLeft->GetAsset())
			JetTailVFXLeft->SetFloatParameter(FName("Lifetime") , 1.f);
		if (JetTailVFXRight && JetTailVFXRight->GetAsset())
			JetTailVFXRight->SetFloatParameter(FName("Lifetime") , 1.f);

		CRPC_AudioControl(true , 2);
		bJetTailVFXOn = false;
	}
#pragma endregion

#pragma region Canopy Open & Close
	if (iCanopyNum == 2)
	{
		if (CanopyPitch > 0)
		{
			if (!bPlayCanopyEndSound)
				bPlayCanopyEndSound = true;
			// 캐노피를 닫는다.
			ServerRPC_Canopy(false);
		}
		else if (CanopyPitch == 0)
		{
			if (bPlayCanopyEndSound)
			{
				CRPC_CanopyAudioControl(true , 2);
				bPlayCanopyEndSound = false;
			}
		}
	}
	else if (iCanopyNum == 0)
	{
		if (CanopyPitch < 80)
		{
			if (!bPlayCanopyEndSound)
				bPlayCanopyEndSound = true;
			// 캐노피를 연다.
			ServerRPC_Canopy(true);
		}
		else if (CanopyPitch == 80)
		{
			if (bPlayCanopyEndSound)
			{
				CRPC_CanopyAudioControl(true , 2);
				bPlayCanopyEndSound = false;
			}
		}
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

		if (IsLocallyControlled())
		{
			if (FrontWheel < 1.f)
				ServerRPC_Wheel();
			else
				IsFlyStart = false;
		}
	}
#pragma endregion

#pragma region 시동 절차 단계
	if (!IsStart)
	{
		if (StartScenario.size() > 0)
		{
#pragma region 시동절차
			FString ScenarioFront = StartScenario.front();
			CurrentScenario = ScenarioFront;
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
						engineProgSuccessDel.Broadcast(EEngineProgress::MIC_SWITCH_ON);
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
						engineProgSuccessDel.Broadcast(EEngineProgress::ENGINE_GEN_SWITCH_ON);
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
						engineProgSuccessDel.Broadcast(EEngineProgress::ENGINE_CONTROL_SWITCH_ON);
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
						engineProgSuccessDel.Broadcast(EEngineProgress::JFS_STARTER_SWITCH_ON);
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
						engineProgSuccessDel.Broadcast(EEngineProgress::ENGINE_MASTER_SWITCH_ON);
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
						engineProgSuccessDel.Broadcast(EEngineProgress::JFS_HANDLE_PULL);
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
					CRPC_EngineSound(true , 2);
					engineProgSuccessDel.Broadcast(EEngineProgress::ENGINE_THROTTLE_IDLE);
					DummyThrottleMesh->SetRenderCustomDepth(false);
					DummyThrottleMesh->CustomDepthStencilValue = 0;
					StartScenario.pop();
				}
			}
			else if (ScenarioFront.Equals("Canopy"))
			{
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
						engineProgSuccessDel.Broadcast(EEngineProgress::CLOSE_CANOPY);
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
					engineProgSuccessDel.Broadcast(EEngineProgress::RELEASE_SIDE_BREAK);
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
		}
	}
#pragma endregion
#pragma region 운행 단계
	else
	{
		CurrentTime += DeltaTime;

		if (bStartAudio)
		{
			CurAudioTime += DeltaTime;
			if (CurAudioTime >= PlayAudioTime)
			{
				if (auto pc = Cast<AJ_MissionPlayerController>(Controller))
				{
					pc->CRPC_SetMissionTextUI(19);
					pc->CRPC_PlayCommanderVoice3(19);
				}
				bStartAudio = false;
			}
		}

		FRotator jetRot = JetArrow->GetRelativeRotation();

#pragma region Move Throttle
		FVector engineLoc = DummyThrottleMesh->GetRelativeLocation();

		if (IsLocallyControlled())
		{
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
							DummyThrottleMesh->SetRelativeLocation(VecTrigger0);
						else
							DummyThrottleMesh->SetRelativeLocation(FVector(newEngineX , engineLoc.Y , engineLoc.Z));
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
							DummyThrottleMesh->SetRelativeLocation(ThrottleMilLoc);
						else
							DummyThrottleMesh->SetRelativeLocation(FVector(newEngineX , engineLoc.Y , engineLoc.Z));
					}
					if (engineLoc.X < VecTrigger1.X)
					{
						auto newEngineX = engineLoc.X + ThrottleMoveSpeed2;
						newEngineX = UKismetMathLibrary::FClamp(newEngineX , ThrottleOffLoc.X , VecTrigger1.X);
						if (VecTrigger1.X - newEngineX < 0.2)
							DummyThrottleMesh->SetRelativeLocation(VecTrigger1);
						else
							DummyThrottleMesh->SetRelativeLocation(FVector(newEngineX , engineLoc.Y , engineLoc.Z));
					}
				}
				else if (intTriggerNum == 2)
				{
					if (engineLoc.X < ThrottleMilLoc.X)
					{
						auto newEngineX = engineLoc.X + ThrottleMoveSpeed1;
						newEngineX = UKismetMathLibrary::FClamp(newEngineX , ThrottleOffLoc.X , ThrottleMilLoc.X);
						if (ThrottleMilLoc.X - newEngineX < 0.2)
							DummyThrottleMesh->SetRelativeLocation(ThrottleMilLoc);
						else
							DummyThrottleMesh->SetRelativeLocation(FVector(newEngineX , engineLoc.Y , engineLoc.Z));
					}
					else if (engineLoc.X < ThrottleMaxLoc.X)
					{
						auto newEngineX = engineLoc.X + ThrottleMoveSpeed2;
						newEngineX = UKismetMathLibrary::FClamp(newEngineX , ThrottleMilLoc.X , ThrottleMaxLoc.X);
						DummyThrottleMesh->SetRelativeLocation(FVector(newEngineX , engineLoc.Y , engineLoc.Z));
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
				DummyThrottleMesh->SetRelativeLocation(FVector(newEngineX , engineLoc.Y , engineLoc.Z));
			}
		}
#pragma endregion

#pragma region Get Accel Gear Number
		if (IsLocallyControlled())
			SetAccelGear();
#pragma endregion

#pragma region Jet Move
		if (IsEngineOn)
		{
			if (IsLocallyControlled())
			{
				//ClientRPCLocation();
				AsyncTask(ENamedThreads::GameThread , [this]()
				{
					ClientRPCLocation();
				});

				// 카메라 쉐이크
				// 활주로를 달리고 있을때가 intTriggerNum < 2 이다.
				if (intTriggerNum < 2 && ValueOfMoveForce > 0)
					CRPC_CameraShake();
			}
		}
#pragma endregion

#pragma region Lock On
		if (bLockOnStart && IsLocallyControlled())
		{
			if (!GroundTarget)
			{
				TArray<AActor*> ActorArray;
				UGameplayStatics::GetAllActorsOfClass(GetWorld() , AJ_GroundTarget::StaticClass() , ActorArray);
				for (AActor* Actor : ActorArray)
				{
					AJ_GroundTarget* MissionActor = Cast<AJ_GroundTarget>(Actor);
					if (MissionActor)
					{
						GroundTarget = MissionActor;
					}
				}
			}

			ClientRPCLockOn();

			if (GroundTarget)
			{
				//auto dist = FVector::Distance(GetActorLocation() , GroundTarget->GetActorLocation());
				auto dist = FMath::Abs(GetActorLocation().X - GroundTarget->GetActorLocation().X);
				if (dist <= 550000)
				{
					CRPC_MissilePitch(2.35f);
				}
				else
				{
					CRPC_MissilePitch(1.f);
				}

				if (!MissileWidget->IsVisible())
				{
					CRPC_MissileCapture();
				}
				else
				{
					CRPC_SetMissileCamRotate();
				}
			}
		}
#pragma endregion

#pragma region Flare Arrow Rotation Change
		// if (CurrentWeapon == EWeapon::Flare)
		// {
		// 	int32 randRot = FMath::RandRange(-150 , -110);
		// 	FRotator newFlareRot = FRotator(randRot , 0 , 0);
		// 	JetFlareArrow3->SetRelativeRotation(newFlareRot);
		// 	JetFlareArrow2->SetRelativeRotation(newFlareRot);
		// }
#pragma endregion
	}
#pragma endregion

	if (IsLocallyControlled())
	{
		ChangeBooster();

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
		float km = ValueOfMoveForce / 100000.f;
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

#pragma region Recover CameraArm Rotation
		if (!IsRotateTrigger && !IsRotateStickTrigger)
		{
			if (JetCamera->IsActive())
			{
				FRotator TPSrot = JetSprintArm->GetRelativeRotation();
				auto lerpTPSrot = FMath::Lerp(TPSrot , TargetArmRotation , DeltaTime);
				JetSprintArm->SetRelativeRotation(lerpTPSrot);
			}
			else
			{
				// FRotator FPSrot = JetSprintArmFPS->GetRelativeRotation();
				// auto lerpFPSrot = FMath::Lerp(FPSrot , FRotator(-30 , 0 , 0) , DeltaTime);
				// JetSprintArmFPS->SetRelativeRotation(lerpFPSrot);
			}
		}
#pragma endregion
	}
}

#pragma region Print Net Log
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
#pragma endregion

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

#pragma region Booster VFX
void AL_Viper::ChangeBooster()
{
	FNiagaraVariable vfxParam(FNiagaraTypeDefinition::GetFloatDef() , FName("EnergyCore_Life"));
	if (IsEngineOn)
	{
		if (AccelGear == 3)
		{
			if (!bCurrBoostState)
				ServerRPCBoost(true);
		}
		else
		{
			if (bCurrBoostState)
				ServerRPCBoost(false);
		}
	}
}

void AL_Viper::ServerRPCBoost_Implementation(bool isOn)
{
	MulticastRPCBoost(isOn);
}

void AL_Viper::MulticastRPCBoost_Implementation(bool isOn)
{
	bCurrBoostState = isOn;
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
#pragma endregion

#pragma region Set Location & Rotation
void AL_Viper::ServerRPC_SyncLocation_Implementation(const FVector& location)
{
	float dis = FVector::Dist(this->GetActorLocation() , location);
	if (dis < 5.f && !this->IsLocallyControlled()) return;

	//this->SetActorLocation(location);
	MultiRPC_SyncLocation(location);
}

void AL_Viper::MultiRPC_SyncLocation_Implementation(const FVector& location)
{
	if (this->IsLocallyControlled()) return;
	if (location.Equals(this->GetActorLocation())) return;
	this->SetActorLocation(location);
}

void AL_Viper::ClientRPCLocation_Implementation()
{
	ValueOfMoveForce += (GetAddTickSpeed() * 2);
	if (ValueOfMoveForce < 0)
		ValueOfMoveForce = 0;
	else if (ValueOfMoveForce > MaxValueOfMoveForce)
		ValueOfMoveForce = MaxValueOfMoveForce;
	//ServerRPCLocation(ValueOfMoveForce);

	// Add Force
	FVector forceVec = JetArrow->GetForwardVector() * ValueOfMoveForce;
	FVector forceLoc = JetRoot->GetComponentLocation();
	if (JetRoot->IsSimulatingPhysics())
		JetRoot->AddForceAtLocation(forceVec , forceLoc);

	// Move Up & Down
	auto jetRot = JetArrow->GetRelativeRotation();
	float zRot = jetRot.Quaternion().Y * jetRot.Quaternion().W * ValueOfHeightForce * 10.f;
	JetRoot->AddForceAtLocation(FVector(0 , 0 , zRot) , HeightForceLoc);
}

void AL_Viper::ServerRPCRotation_Implementation(FQuat newQuat)
{
	if (bJetAirVFXOn)
	{
		if (GetActorRotation().Pitch > 10 && QuatCurrentRotation.Rotator().Pitch <= newQuat.Rotator().Pitch)
			MultiRPCVisibleAirVFX(true);
		else
			MultiRPCVisibleAirVFX(false);
	}

	QuatCurrentRotation = newQuat;
	QuatTargetRotation = newQuat;
	SetActorRotation(QuatCurrentRotation.Rotator());
	MultiRPCRotation(newQuat);
}

void AL_Viper::MultiRPCRotation_Implementation(const FQuat& newQuat)
{
	if (IsLocallyControlled()) return;

	QuatCurrentRotation = newQuat;
	QuatTargetRotation = newQuat;
	SetActorRotation(QuatCurrentRotation);
}

void AL_Viper::MultiRPCVisibleAirVFX_Implementation(bool isOn)
{
	AirResistanceVFX->SetVisibility(isOn);
}
#pragma endregion

#pragma region Projectile
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
			}
		}
		else
		{
			//LOG_S(Warning , TEXT("미사일 액터가 없습니다."));
		}
	}
	else
	{
		//LOG_S(Warning , TEXT("타겟이 없습니다!!"));
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
			//LOG_S(Warning , TEXT("남은 Flare가 없습니다."));
		}
	}
	else
	{
		//LOG_S(Warning , TEXT("FlareFactory가 없습니다."));
	}
}

void AL_Viper::ClientRPCLockOn_Implementation()
{
	if (!bStartLockOn)
	{
		CRPC_MissileSound(true);
		bStartLockOn = true;
	}

#pragma region Lock On
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
		                                           false , Overlaps , EDrawDebugTrace::None , OutHit , true))
		{
			for (auto hit : OutHit)
			{
				if (auto mai = Cast<IJ_MissionActorInterface>(hit.GetActor()))
				{
					if (!LockOnTarget)
						PlayLockOnSound();
					searchTarget = hit.GetActor();
				}
			}
		}
	}
#pragma endregion

	if ((LockOnTarget && !searchTarget) || (!LockOnTarget && searchTarget))
	{
		ServerRPCLockOn(searchTarget);

		if (searchTarget)
		{
			CRPC_MissileSound(false);
		}
		else
		{
			CRPC_MissileSound(true);
		}
	}

#pragma region Show Lock On UI
	if (searchTarget && TargetUIActorFac)
	{
		if (TargetUIActorFac)
		{
			if (!TargetActor)
			{
				FActorSpawnParameters SpawnParams;
				SpawnParams.Owner = this;
				SpawnParams.Instigator = GetInstigator();

				FVector TargetLocation = searchTarget->GetActorLocation() + FVector(0 , 0 , 100);
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
		}
	}
#pragma endregion

	Diametr = 30.f;
}

void AL_Viper::ServerRPCLockOn_Implementation(AActor* target)
{
	// AActor* searchTarget = nullptr;
	// FVector Start = JetMesh->GetComponentLocation();
	// FVector ForwardVector = JetMesh->GetForwardVector();
	// FVector DownVector = JetMesh->GetUpVector() * -1;
	//
	// TArray<AActor*> Overlaps;
	// TArray<FHitResult> OutHit;
	// for (int i = 0; i < RangeCnt; i++)
	// {
	// 	Diametr *= 2.f;
	// 	Start += (ForwardVector * Diametr / 4) + (DownVector * Diametr / 2);
	// 	if (UKismetSystemLibrary::SphereTraceMulti(GetWorld() , Start , Start , Diametr / 2.f , TraceTypeQuery1 ,
	// 											   false , Overlaps , EDrawDebugTrace::None , OutHit , true))
	// 	{
	// 		for (auto hit : OutHit)
	// 		{
	// 			if (auto mai = Cast<IJ_MissionActorInterface>(hit.GetActor()))
	// 			{
	// 				if (!LockOnTarget)
	// 					ClientRPCLockOnSound(this);
	// 				searchTarget = hit.GetActor();
	// 			}
	// 		}
	// 	}
	// }

	MulticastRPCLockOn(target);
	// ClientRPCSetLockOnUI(this , searchTarget);

	// Diametr = 30.f;
}

void AL_Viper::MulticastRPCLockOn_Implementation(AActor* target)
{
	LockOnTarget = target;
}

void AL_Viper::CRPC_MissileCapture_Implementation()
{
	MissileWidget->SetVisibility(true);

	if (JetCameraMissileCam)
	{
		UMaterialInstanceDynamic* DynMaterial = UMaterialInstanceDynamic::Create(MissileSceneMat , this);
		UTextureRenderTarget2D* MyRenderTarget = UKismetRenderingLibrary::CreateRenderTarget2D(
			this , 2048 , 2048 , ETextureRenderTargetFormat::RTF_RGBA32f , FLinearColor::White);
		JetCameraMissileCam->TextureTarget = MyRenderTarget;
		DynMaterial->SetTextureParameterValue(FName("MissileScene") , JetCameraMissileCam->TextureTarget);

		if (auto camui = Cast<UL_MissileCam>(MissileWidget->GetWidget()))
		{
			// UI의 Image 위젯에 설정
			if (camui->img_cam)
			{
				// DynamicMaterial를 UI에 적용시키기 위해 FSlateBrush를 사용해야함
				FSlateBrush Brush;
				Brush.SetResourceObject(DynMaterial);
				// 위잿 내부에 있는 이미지 변경
				camui->img_cam->SetBrush(Brush);
			}
		}
	}
}

void AL_Viper::PlayLockOnSound()
{
	if (LockOnSound)
		UGameplayStatics::PlaySound2D(this , LockOnSound);
}

void AL_Viper::ClientRPC_LockOnStart_Implementation()
{
	bLockOnStart = true;
}

void AL_Viper::CRPC_SetMissileCamRotate_Implementation()
{
	auto distance = GroundTarget->GetActorLocation() - JetCameraMissileCam->GetComponentLocation();
	auto rot = distance.Rotation();
	JetCameraMissileCam->SetRelativeRotation(rot.Quaternion());
}
#pragma endregion

#pragma region Set Gear Number
void AL_Viper::SetAccelGear()
{
	// 기어 변동 구간
	// 0%, 50%, 90%
	auto currAccelGear = DummyThrottleMesh->GetRelativeLocation().X;
	auto currValue = currAccelGear - ThrottleOffLoc.X;
	auto SizeValue = ThrottleMaxLoc.X - ThrottleOffLoc.X;
	auto per = currValue / SizeValue * 100;
	if (per <= 5)
	{
		AccelGear = 0;
	}
	else if (per <= 50)
	{
		AccelGear = 1;
	}
	else if (per <= 90)
	{
		AccelGear = 2;
	}
	else
	{
		AccelGear = 3;
	}
}
#pragma endregion

#pragma region Right Mouse Click
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
				if (HitResult.GetComponent()->ComponentHasTag("JetCanopy"))
				{
					BackMoveCanopyHandle();
					// 디버그용 라인 시각화
					//DrawDebugLine(GetWorld() , Start , End , FColor::Green , false , 2.0f , 0 , 2.0f);
				}
			}
		}
	}
}
#pragma endregion

#pragma region Move Canopy Hnd
void AL_Viper::BackMoveCanopyHandle()
{
	CRPC_PlaySwitchSound(DummyCanopyMesh->GetComponentLocation());

	auto currLoc = DummyCanopyMesh->GetRelativeLocation();

	if (FVector::Dist(currLoc , CanopyHoldLoc) <= 1)
	{
		DummyCanopyMesh->SetRelativeLocation(CanopyCloseLoc);
		if (CanopyPitch > 0)
			CRPC_CanopyAudioControl(true , 1);
		iCanopyNum = 2;
	}
	else if (FVector::Dist(currLoc , CanopyCloseLoc) <= 1)
	{
		DummyCanopyMesh->SetRelativeLocation(CanopyNormalLoc);
		CRPC_CanopyAudioControl(false);
		iCanopyNum = 1;
	}
	else if (FVector::Dist(currLoc , CanopyNormalLoc) <= 1)
	{
		DummyCanopyMesh->SetRelativeLocation(CanopyOpenLoc);
		if (CanopyPitch < 80)
			CRPC_CanopyAudioControl(true , 0);
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
		//LOG_S(Warning , TEXT("Open %f") , newPitch);
	}
	else
	{
		// 캐노피가 닫힌다.
		float newPitch = CanopyPitch - CanopyRotatePitchValue;
		newPitch = FMath::Clamp(newPitch , 0.f , 80.f);
		CanopyPitch = newPitch;
		//LOG_S(Warning , TEXT("Close %f") , newPitch);
	}
}
#pragma endregion

#pragma region Landing Gear
void AL_Viper::ServerRPC_Wheel_Implementation()
{
	float DeltaTime = GetWorld()->DeltaTimeSeconds;
	float nowValue = FMath::Lerp(FrontWheel , 1 , DeltaTime);

	if (nowValue >= 0.9)
		nowValue = 1;

	FrontWheel = nowValue;
	RearWheel = nowValue;
}
#pragma endregion

#pragma region Voice Chat
void AL_Viper::StartVoiceChat()
{
	GetController<AJ_MissionPlayerController>()->StartTalking();
}

void AL_Viper::StopVoiceChat()
{
	GetController<AJ_MissionPlayerController>()->StopTalking();
}
#pragma endregion

#pragma region KHS Works
void AL_Viper::ServerRPC_SetConnectedPlayerNames_Implementation(const FString& newName)
{
	auto KGameState = CastChecked<AK_GameState>(UGameplayStatics::GetGameState(GetWorld()));
	if (!KGameState)
	{
		// LOG_S(Warning , TEXT("GameState doesn't exist"));
	}
	// LOG_S(Warning , TEXT("MyUserID : %s") , *newName);

	//GameState의 ConnectedPlayerNames 배열에 자신의 ID Set(Replicated)
	KGameState->SetConnectedPlayerNames(newName);

	//GameState에 업데이트된 배열 가져옴
	TArray<FString> temp = KGameState->GetConnectedPlayernames();

	//로그출력
	for (auto s : temp)
	{
		// LOG_S(Warning , TEXT("The Name in PlayerList : %s") , *s);
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

#pragma region SFX
void AL_Viper::CRPC_CanopyAudioControl_Implementation(bool bStart , int32 idx)
{
	if (bStart)
	{
		if (JetCanopyAudio && JetCanopyAudio->GetSound())
		{
			JetCanopyAudio->SetIntParameter("JetSoundIdx" , idx);
			JetCanopyAudio->Play(0.f);
		}
	}
	else
	{
		if (JetCanopyAudio && JetCanopyAudio->GetSound())
			JetCanopyAudio->Stop();
	}
}

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
		// LOG_S(Warning , TEXT("GameState doesn't exist"));
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

void AL_Viper::CRPC_MICAudioControl_Implementation(bool bStart)
{
	if (bStart)
	{
		if (JetMICAudio && JetMICAudio->GetSound())
		{
			JetMICAudio->SetIntParameter("JetSoundIdx" , 0);
			JetMICAudio->Play(0.f);
		}
	}
	else
	{
		if (JetMICAudio && JetMICAudio->GetSound())
			JetMICAudio->Stop();
	}
}

void AL_Viper::CRPC_EngineSound_Implementation(bool bStart , int32 idx)
{
	if (bStart)
	{
		if (JetEngineAudio && JetEngineAudio->GetSound())
		{
			JetEngineAudio->SetIntParameter("JetSoundIdx" , idx);
			JetEngineAudio->Play(0.f);

			if (idx == 0)
			{
				FTimerHandle Thnd;
				GetWorld()->GetTimerManager().SetTimer(Thnd , [&]()
				{
					JetEngineAudio->SetIntParameter("JetSoundIdx" , 1);
					JetEngineAudio->Play(0.f);
				} , 3.4f , false);
			}
		}
	}
	else
	{
		if (JetEngineAudio && JetEngineAudio->GetSound())
			JetEngineAudio->Stop();
	}
}

void AL_Viper::CRPC_AirSound_Implementation(bool bStart)
{
	if (bStart)
	{
		if (JetAirAudio && JetAirAudio->GetSound())
		{
			JetAirAudio->Play(0.f);
		}
	}
	else
	{
		if (JetAirAudio && JetAirAudio->GetSound())
			JetAirAudio->Stop();
	}
}

void AL_Viper::CRPC_MissileSound_Implementation(bool bStart)
{
	if (bStart)
	{
		if (JetMissileAudio && JetMissileAudio->GetSound())
		{
			JetMissileAudio->SetIntParameter("MissileIdx" , 2);
			JetMissileAudio->Play(0.f);
		}
	}
	else
	{
		if (JetMissileAudio && JetMissileAudio->GetSound())
			JetMissileAudio->Stop();
	}
}

void AL_Viper::CRPC_MissilePitch_Implementation(const float& Pitch)
{
	if (JetMissileAudio)
	{
		JetMissileAudio->SetPitchMultiplier(Pitch);
	}
}
#pragma endregion

#pragma region Camera Shake
void AL_Viper::CRPC_CameraShake_Implementation()
{
	if (LoadCameraShake)
		UGameplayStatics::PlayWorldCameraShake(GetWorld() , LoadCameraShake , GetActorLocation() , 300.f , 700.f);
}
#pragma endregion

#pragma region Throttle Machine
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
	// LOG_S(Warning , TEXT("F_ThrottleAxis4 : %f") , data);

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
		DummyThrottleMesh->
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
			engineProgSuccessDel.Broadcast(EEngineProgress::ENGINE_THROTTLE_IDLE);
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
		DummyThrottleMesh->
			SetRelativeLocation(FVector(ThrottleOffLoc.X + moveValue , ThrottleOffLoc.Y , ThrottleOffLoc.Z));
	}
	DeviceThrottleCurrentValue = data;
}

void AL_Viper::F_ThrottleAxis6(const struct FInputActionValue& value)
{
	float data = value.Get<float>();
	if (data > .8f)
	{
		bCanopyNormalSound = false;
		bCanopyOpenSound = false;
		bCanopyCloseSound = false;
		// 잠금
		DummyCanopyMesh->SetRelativeLocation(CanopyHoldLoc);
		if (!bCanopyHoldSound)
		{
			bCanopyHoldSound = true;
			CRPC_CanopyAudioControl(true , 1);
		}
		iCanopyNum = 3;
	}
	else if (data > .6f)
	{
		bCanopyHoldSound = false;
		bCanopyNormalSound = false;
		bCanopyOpenSound = false;
		// 닫기
		DummyCanopyMesh->SetRelativeLocation(CanopyCloseLoc);
		if (!bCanopyCloseSound && CanopyPitch > 0)
		{
			bCanopyCloseSound = true;
			CRPC_CanopyAudioControl(true , 0);
		}
		iCanopyNum = 2;
	}
	else if (data > .3f)
	{
		bCanopyHoldSound = false;
		bCanopyOpenSound = false;
		bCanopyCloseSound = false;
		// 중립
		DummyCanopyMesh->SetRelativeLocation(CanopyNormalLoc);
		if (!bCanopyNormalSound)
		{
			bCanopyNormalSound = true;
			CRPC_CanopyAudioControl(false);
		}
		iCanopyNum = 1;
	}
	else
	{
		bCanopyHoldSound = false;
		bCanopyNormalSound = false;
		bCanopyNormalSound = false;
		// 열기
		DummyCanopyMesh->SetRelativeLocation(CanopyOpenLoc);
		if (!bCanopyOpenSound && CanopyPitch < 80)
		{
			bCanopyOpenSound = true;
			CRPC_CanopyAudioControl(true , 0);
		}
		iCanopyNum = 0;
	}
}
#pragma endregion

#pragma region Stick Machine
void AL_Viper::F_StickButton1Started(const struct FInputActionValue& value)
{
	CurrentWeapon = static_cast<EWeapon>((static_cast<int32>(CurrentWeapon) + 1) % static_cast<int32>(EWeapon::Max));
}

void AL_Viper::F_StickButton2Started(const struct FInputActionValue& value)
{
	if (CurrentWeapon == EWeapon::Missile)
		ServerRPCMissile(this);
	else if (CurrentWeapon == EWeapon::Flare)
		ServerRPCFlare(this);
}

void AL_Viper::F_StickButton5Started(const struct FInputActionValue& value)
{
	if (JetCamera && JetCamera->IsActive())
	{
		JetCamera->SetActive(false);
		JetSprintArm->PrimaryComponentTick.bCanEverTick = false;
		JetCamera->PrimaryComponentTick.bCanEverTick = false;
		if (JetCameraFPS)
		{
			if (JetPostProcess && JetPostProcess->Settings.WeightedBlendables.Array.Num() > 0)
				JetPostProcess->Settings.WeightedBlendables.Array[0].Weight = 1;
			JetCameraFPS->SetActive(true);
			JetSprintArmFPS->PrimaryComponentTick.bCanEverTick = true;
			JetCameraFPS->PrimaryComponentTick.bCanEverTick = true;
		}
	}
	else
	{
		if (JetCamera)
		{
			JetCamera->SetActive(true);
			JetSprintArm->PrimaryComponentTick.bCanEverTick = true;
			JetCamera->PrimaryComponentTick.bCanEverTick = true;
			if (JetCameraFPS)
			{
				if (JetPostProcess && JetPostProcess->Settings.WeightedBlendables.Array.Num() > 0)
					JetPostProcess->Settings.WeightedBlendables.Array[0].Weight = 0;
				JetCameraFPS->SetActive(false);
				JetSprintArmFPS->PrimaryComponentTick.bCanEverTick = false;
				JetCameraFPS->PrimaryComponentTick.bCanEverTick = false;
			}
		}
	}
}

void AL_Viper::F_StickButton11Started(const struct FInputActionValue& value)
{
	if (!IsZoomOut)
		IsZoomIn = true;
}

void AL_Viper::F_StickButton11Completed(const struct FInputActionValue& value)
{
	// 중복 키입력 방지용
	IsZoomIn = false;
}

void AL_Viper::F_StickButton13Started(const struct FInputActionValue& value)
{
	if (!IsZoomIn)
		IsZoomOut = true;
}

void AL_Viper::F_StickButton13Completed(const struct FInputActionValue& value)
{
	IsZoomOut = false;
}

void AL_Viper::F_StickAxis1(const struct FInputActionValue& value)
{
	float data = value.Get<float>();
	data = FMath::RoundToFloat(data * 1000.0f) / 1000.0f;
	FString strData = FString::Printf(TEXT("%.3f") , data);

	float X = 0;
	float Y = 0;
	if (strData.Equals("3.286"))
	{
		IsRotateStickTrigger = false;
		return;
	}
	else if (strData.Equals("-1.000"))
		Y = -1;
	else if (strData.Equals("-0.714"))
	{
		X = -1;
		Y = -1;
	}
	else if (strData.Equals("-0.429"))
		X = -1;
	else if (strData.Equals("-0.143"))
	{
		X = -1;
		Y = 1;
	}
	else if (strData.Equals("0.143"))
		Y = 1;
	else if (strData.Equals("0.429"))
	{
		X = 1;
		Y = 1;
	}
	else if (strData.Equals("0.714"))
		X = 1;
	else if (strData.Equals("1.000"))
	{
		X = 1;
		Y = -1;
	}
	IsRotateStickTrigger = true;

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
}

void AL_Viper::F_StickAxis3(const struct FInputActionValue& value)
{
	// Left(-1), Right(1)
	float data = value.Get<float>();
	StickRollAngle = data * -1;

	if ((StickRollAngle > 0.6f || StickRollAngle < -0.6f) && (StickPitchAngle > 0.6f || StickPitchAngle < -0.6f))
	{
		StickRollAngle = StickRollAngle * MaxRotationAngle / 3.f;
		StickPitchAngle = StickPitchAngle * MaxRotationAngle / 5.f;
	}
	else
	{
		StickRollAngle = StickRollAngle * MaxRotationAngle / 5.f;
		StickPitchAngle = StickPitchAngle * MaxRotationAngle / 8.f;
	}

	// Roll과 Pitch를 쿼터니언 회전으로 변환
	FQuat RollRotation = FQuat(FVector(1 , 0 , 0) , FMath::DegreesToRadians(StickRollAngle));
	FQuat PitchRotation = FQuat(FVector(0 , 1 , 0) , FMath::DegreesToRadians(StickPitchAngle));

	// 목표 회전 설정 (RootComponent를 기준으로)
	QuatTargetRotation = QuatCurrentRotation * RollRotation * PitchRotation;
	StickRollAngle = 0.f;
	StickPitchAngle = 0.f;

#pragma region Retate Pawn
	// 현재 회전을 목표 회전으로 보간 (DeltaTime과 RotationSpeed를 사용하여 부드럽게)
	QuatCurrentRotation = FQuat::Slerp(QuatCurrentRotation , QuatTargetRotation ,
	                                   RotationSpeed * GetWorld()->GetDeltaSeconds());

	// 서버일때 회전을 하면 SRPC에서 한번 더 회전되기 때문에 문제될 수 있다. 
	if (!HasAuthority())
		SetActorRotation(QuatTargetRotation);
	ServerRPCRotation(QuatTargetRotation);

	if (bJetAirVFXOn)
	{
		if (GetActorRotation().Pitch > 10 && QuatCurrentRotation.Rotator().Pitch <= QuatTargetRotation.Rotator().Pitch)
			CRPC_AirSound(true);
		else
			CRPC_AirSound(false);
	}
#pragma endregion
}
#pragma endregion

#pragma region VR Stick
void AL_Viper::VRSticAxis(const FVector2D& value)
{
	VRStickCurrentPitchValue = value.Y;
	VRStickCurrentRollValue = -1 * value.X;
	float VRStickRollAngle = 0.f;
	float VRStickPitchAngle = 0.f;

	if ((VRStickCurrentRollValue > VRStickMaxThreshold || VRStickCurrentRollValue < VRStickMinThreshold) && (
		VRStickCurrentPitchValue >
		VRStickMaxThreshold || VRStickCurrentPitchValue < VRStickMinThreshold))
	{
		VRStickRollAngle = VRStickCurrentRollValue * MaxRotationAngle / VRStickBankRollDiv;
		VRStickPitchAngle = VRStickCurrentPitchValue * MaxRotationAngle / VRStickBankPitchDiv;
	}
	else
	{
		VRStickRollAngle = VRStickCurrentRollValue * MaxRotationAngle / VRStickkRollDiv;
		VRStickPitchAngle = VRStickCurrentPitchValue * MaxRotationAngle / VRStickPitchDiv;
	}
	// Roll과 Pitch를 쿼터니언 회전으로 변환
	FQuat RollRotation = FQuat(FVector(1 , 0 , 0) , FMath::DegreesToRadians(VRStickRollAngle));
	FQuat PitchRotation = FQuat(FVector(0 , 1 , 0) , FMath::DegreesToRadians(VRStickPitchAngle));

	// 목표 회전 설정 (RootComponent를 기준으로)
	QuatTargetRotation = QuatCurrentRotation * RollRotation * PitchRotation;
	VRStickCurrentPitchValue = 0.f;
	VRStickCurrentRollValue = 0.f;

#pragma region Retate Pawn
	// 현재 회전을 목표 회전으로 보간 (DeltaTime과 RotationSpeed를 사용하여 부드럽게)
	QuatCurrentRotation = FQuat::Slerp(QuatCurrentRotation , QuatTargetRotation ,
	                                   RotationSpeed * GetWorld()->GetDeltaSeconds());

	// 서버일때 회전을 하면 SRPC에서 한번 더 회전되기 때문에 문제될 수 있다. 
	if (!HasAuthority())
		SetActorRotation(QuatTargetRotation);
	ServerRPCRotation(QuatTargetRotation);

	if (bJetAirVFXOn)
	{
		if (GetActorRotation().Pitch > 10 && QuatCurrentRotation.Rotator().Pitch <= QuatTargetRotation.Rotator().Pitch)
			CRPC_AirSound(true);
		else
			CRPC_AirSound(false);
	}
#pragma endregion
}
#pragma endregion

#pragma region IsEngineOn 변수를 true로 전환
void AL_Viper::SetEngineOn()
{
	IsEngineOn = true;
}
#pragma endregion

void AL_Viper::CRPC_TeleportSetting_Implementation()
{
	bJetTailVFXOn = true;
	bJetAirVFXOn = true;
	IsEngineOn = false;
}

void AL_Viper::SRPC_SetMyName_Implementation(const FString& PlayerName)
{
	MRPC_SetMyName(PlayerName);	
}

void AL_Viper::MRPC_SetMyName_Implementation(const FString& PlayerName)
{
	LOG_S(Warning, TEXT("MRPC_SetMyName Start"));
	if (auto PlayerNameWidget = Cast<UL_PlayerNameWidget>(PlayerNameWidgetComponent->GetWidget()))
	{
		LOG_S(Warning, TEXT("%s Player Name : %s"), *this->GetName(), *PlayerName);		
		PlayerNameWidget->txtPlayerName->SetText(FText::FromString(PlayerName));
	}
}

void AL_Viper::SRPC_VisiblePlayerName_Implementation()
{
	MRPC_VisiblePlayerName();
}

void AL_Viper::MRPC_VisiblePlayerName_Implementation()
{
	PlayerNameWidgetComponent->SetHiddenInGame(false);
	PlayerNameWidgetComponent->SetVisibility(true);
}

void AL_Viper::StopAllVoice()
{
	if (JetAudio && JetAudio->GetSound())
		JetAudio->Stop();
	if (JetMICAudio && JetMICAudio->GetSound())
		JetMICAudio->Stop();
	if (JetEngineAudio && JetEngineAudio->GetSound())
		JetEngineAudio->Stop();
	if (JetAirAudio && JetAirAudio->GetSound())
	{
		JetAirAudio->Stop();
		JetAirAudio->SetSound(nullptr);
	}
	if (JetMissileAudio && JetMissileAudio->GetSound())
	{
		JetMissileAudio->Stop();
		JetMissileAudio->SetSound(nullptr);
	}
}

void AL_Viper::StickRotation()
{
	float StickCurRoll = DummyStick->GetRelativeRotation().Roll;
	float StickCurPitch = DummyStick->GetRelativeRotation().Pitch;

	auto RollValue = FMath::Abs(StickCurRoll);
	auto PitchValue = FMath::Abs(StickCurPitch);

	auto RollPer = RollValue / StickMaxRoll;
	auto PitchPer = PitchValue / StickMinPitch;

	if (StickCurRoll < 0)
		RollPer *= -1;
	if (StickCurPitch < 0)
		PitchPer *= -1;

	float RollAngle = 0.f;
	float PitchAngle = 0.f;
	float StickMinThreshold = StickMaxThreshold * -1;

	if ((RollPer > StickMaxThreshold || RollPer < StickMinThreshold) && (PitchPer > StickMaxThreshold || PitchPer <
		StickMinThreshold))
	{
		RollAngle = RollPer * MaxRotationAngle / StickDivRoll;
		PitchAngle = PitchPer * MaxRotationAngle / StickDivPitch;
	}
	else
	{
		RollAngle = RollPer * MaxRotationAngle / StickDivBankRoll;
		PitchAngle = PitchPer * MaxRotationAngle / StickDivBankPitch;
	}

	// Roll과 Pitch를 쿼터니언 회전으로 변환
	FQuat RollRotation = FQuat(FVector(1 , 0 , 0) , FMath::DegreesToRadians(RollAngle));
	FQuat PitchRotation = FQuat(FVector(0 , 1 , 0) , FMath::DegreesToRadians(PitchAngle));

	// 목표 회전 설정 (RootComponent를 기준으로)
	QuatTargetRotation = QuatCurrentRotation * RollRotation * PitchRotation;
	RollAngle = 0.f;
	PitchAngle = 0.f;
}

void AL_Viper::SetCanopyGearLevel()
{
	auto currLoc = DummyCanopyMesh->GetRelativeLocation();
	if (FVector::Dist(currLoc , CanopyHoldLoc) <= 1)
	{
		CRPC_CanopyAudioControl(true , 1);
		iCanopyNum = 3;
	}
	if (FVector::Dist(currLoc , CanopyCloseLoc) <= 1)
	{
		CRPC_CanopyAudioControl(true , 1);
		iCanopyNum = 2;
	}
	else if (FVector::Dist(currLoc , CanopyNormalLoc) <= 1)
	{
		CRPC_CanopyAudioControl(true , 0);
		iCanopyNum = 1;
	}
	else if (FVector::Dist(currLoc , CanopyOpenLoc) <= 1)
	{
		CRPC_CanopyAudioControl(false);
		iCanopyNum = 0;
	}
}
