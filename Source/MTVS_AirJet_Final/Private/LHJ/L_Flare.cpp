// Fill out your copyright notice in the Description page of Project Settings.


#include "LHJ/L_Flare.h"

#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
AL_Flare::AL_Flare()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FlareMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FlareMesh"));
	SetRootComponent(FlareMesh);

	FlareBoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("FlareBoxComp"));
	FlareBoxComp->SetupAttachment(RootComponent);
	FlareBoxComp->OnComponentBeginOverlap.AddDynamic(this , &AL_Flare::OnFlareBeginOverlap);

	SetLifeSpan(10.f);

	bReplicates = true;
	SetReplicateMovement(true);

	// 발사체 컴포넌트 등록
	movementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementComp"));
	// 비활성화(throw할때만 활성화되도록)
	movementComp->bAutoActivate = true;
	// movement컴포넌트가 갱신시킬 컴포넌트 지정
	movementComp->UpdatedComponent=FlareMesh;
	movementComp->SetUpdatedComponent(FlareMesh);
	// 발사체 초기속도 설정
	movementComp->InitialSpeed = 10000000.0f;
	// 발사체 최대속도 설정
	movementComp->MaxSpeed = 10000000.0f;
	// 회전 업데이트 설정(발사체회전이 Velocity방향과 일치)
	movementComp->bRotationFollowsVelocity = true;
	movementComp->bShouldBounce=false;
}

// Called when the game starts or when spawned
void AL_Flare::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AL_Flare::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

UProjectileMovementComponent* AL_Flare::GetProjectileMovementComponent() const
{
	return movementComp;
}

void AL_Flare::OnFlareBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ServerRPCFlare();
}

void AL_Flare::ServerRPCFlare_Implementation()
{
}

