// Fill out your copyright notice in the Description page of Project Settings.


#include "KHS/K_EnterenceZone.h"
#include "KHS/K_GameInstance.h"
#include "KHS/K_PlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SphereComponent.h"
#include "MTVS_AirJet_FinalCharacter.h"

// Sets default values
AK_EnterenceZone::AK_EnterenceZone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SetRootComponent(SphereComp);
	SphereComp->SetSphereRadius(100.0f);

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(SphereComp);
	MeshComp->SetCollisionProfileName(TEXT("NoCollision"));

	//TA쪽에서 작업한 메시나 MT가 있으면 그때 추가
	/*ConstructorHelpers::FObjectFinder<UStaticMesh> tempMesh(TEXT("/Script/Engine.StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	if ( tempMesh.Succeeded() )
	{
		compMesh->SetStaticMesh(tempMesh.Object);
	}
	ConstructorHelpers::FObjectFinder<UMaterial> tempMat(TEXT("/Script/Engine.Material'/Game/StarterContent/Materials/M_Metal_Gold.M_Metal_Gold'"));
	if ( tempMat.Succeeded() )
	{
		compMesh->SetMaterial(0 , tempMat.Object);
	}*/
}

// Called when the game starts or when spawned
void AK_EnterenceZone::BeginPlay()
{
	Super::BeginPlay();
	
	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &AK_EnterenceZone::OnMySphereBeginOverlap);
}

// Called every frame
void AK_EnterenceZone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AK_EnterenceZone::OnMySphereBeginOverlap(UPrimitiveComponent* OverlappedComponent , AActor* OtherActor , UPrimitiveComponent* OtherComp , int32 OtherBodyIndex , bool bFromSweep , const FHitResult& SweepResult)
{
	if ( !bIsEntered && OtherActor->IsA<AMTVS_AirJet_FinalCharacter>() )
	{
		//플레이어의 인풋을 제어하고

		auto pc = Cast<AK_PlayerController>(GetWorld()->GetFirstPlayerController());
		if ( nullptr == pc )
		{
			UE_LOG(LogTemp , Warning , TEXT("Player Controller is null"));
			return;
		}
		pc->SetInputMode(FInputModeUIOnly()); //인풋제어

		//플레이어 이동을 멈추도록 함
		auto theActor = Cast<AMTVS_AirJet_FinalCharacter>(OtherActor);
		theActor->GetCharacterMovement()->DisableMovement();

		//SessionUI를 호출하고싶다.
		auto gi = CastChecked<UK_GameInstance>(GetWorld()->GetGameInstance());
		if ( nullptr == gi )
		{
			UE_LOG(LogTemp , Warning , TEXT("ServerWidget is null"));
			return;
		}
		gi->CreateServerWidget(); //ServerWidget생성
		UE_LOG(LogTemp , Warning , TEXT("Create ServerWidget"));

		bIsEntered = true;//플래그 체크
		//-> 나중에 플레이어쪽에서 tick으로 충돌감지하다가 벗어나면 false값 다시 전달하도록 세팅해야함.
	}
}



