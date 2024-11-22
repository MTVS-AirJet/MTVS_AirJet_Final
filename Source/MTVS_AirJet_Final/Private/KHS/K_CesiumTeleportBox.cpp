// Fill out your copyright notice in the Description page of Project Settings.


#include "KHS/K_CesiumTeleportBox.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "JBS/J_MissionPlayerController.h"
#include "JBS/J_Utility.h"
#include "JBS/J_MissionGamemode.h"
#include "LHJ/L_Viper.h"
#include "KHS/K_PlayerController.h"
#include "KHS/K_GameInstance.h"
#include "CesiumGeoreference.h"
//#include "CesiumCartographicPolygon.h"
#include "Kismet/GameplayStatics.h"
#include "Cesium3DTileset.h"

// Sets default values
AK_CesiumTeleportBox::AK_CesiumTeleportBox()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	SetRootComponent(BoxComp);
	BoxComp->SetRelativeScale3D(FVector(200.f, 200.f, 50.f));

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(BoxComp);
	MeshComp->SetCollisionProfileName(TEXT("NoCollision"));


}

// Called when the game starts or when spawned
void AK_CesiumTeleportBox::BeginPlay()
{
	Super::BeginPlay();


} 

// Called every frame
void AK_CesiumTeleportBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AK_CesiumTeleportBox::OnMyBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent , AActor* OtherActor , UPrimitiveComponent* OtherComp , int32 OtherBodyIndex , bool bFromSweep , const FHitResult& SweepResult)
{
	//if ( OtherActor->IsA<AL_Viper>() )
	//{
	//	//LoadingUI를 호출하고싶다.
	//	auto gi = CastChecked<UK_GameInstance>(GetWorld()->GetGameInstance());
	//	if ( nullptr == gi )
	//	{
	//		UE_LOG(LogTemp , Warning , TEXT("ServerWidget is null"));
	//		return;
	//	}
	//	gi->CreateLoadingWidget(); //LoadingWidget생성
	//	UE_LOG(LogTemp , Warning , TEXT("Create ServerWidget"));



	//	
	//}
}

//인스턴스에서 가지고 있던 위경도 조건을 가져오는 함수
void AK_CesiumTeleportBox::SetDestinationLogitudeLatitude(double Logitude , double Latitude)
{
	DestLongitude = Logitude;
	DestLatitude = Latitude;
}

//저장된 변수를 입력받아서 플레이어와 월드폴리곤위치를 옮기는 함수
void AK_CesiumTeleportBox::MovePlayerandPolygonToDestination(double Longitude , double Latitude)
{
    //// Georeference 객체를 참조합니다.
    //WorldGeoReference = GetWorld()->SpawnActor<ACesiumGeoreference>();

    //if ( !WorldGeoReference )
    //{
    //    UE_LOG(LogTemp , Warning , TEXT("Georeference 객체를 생성하지 못했습니다."));
    //    return;
    //}

    //// 위경도 좌표를 Unreal 좌표로 변환합니다.
    //FVector UnrealPosition = WorldGeoReference->TransformLongitudeLatitudeHeightToUnreal(FVector(Longitude , Latitude , 0.0)); // 고도는 0으로 설정

    //// 1. 폴리곤을 찾거나 생성하고 위치를 업데이트합니다.
    //ACesiumCartographicPolygon* CartographicPolygon = nullptr;

    //// 레벨에 이미 존재하는 폴리곤을 찾습니다.
    //for ( TActorIterator<ACesiumCartographicPolygon> It(GetWorld()); It; ++It )
    //{
    //    CartographicPolygon = *It;
    //    break; // 첫 번째 폴리곤을 사용
    //}

    //// 존재하는 폴리곤이 없으면 새로운 폴리곤을 생성합니다.
    //if ( !CartographicPolygon )
    //{
    //    CartographicPolygon = GetWorld()->SpawnActor<ACesiumCartographicPolygon>();
    //}

    //if ( CartographicPolygon )
    //{
    //    // 폴리곤의 위치를 설정합니다.
    //    CartographicPolygon->SetActorLocation(UnrealPosition);

    //    // 폴리곤의 스케일을 설정합니다.
    //    FVector NewScale(2.0f , 2.0f , 1.0f); // X, Y 축을 2배로, Z 축은 그대로 유지
    //    CartographicPolygon->SetActorScale3D(NewScale);
    //}
    //else
    //{
    //    UE_LOG(LogTemp , Warning , TEXT("CartographicPolygon을 찾거나 생성하지 못했습니다."));
    //}

    //// 2. 플레이어의 위치를 업데이트합니다.
    //APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld() , 0);
    //if ( PlayerController )
    //{
    //    APawn* PlayerPawn = PlayerController->GetPawn();
    //    if ( PlayerPawn )
    //    {
    //        PlayerPawn->SetActorLocation(UnrealPosition);
    //    }
    //    else
    //    {
    //        UE_LOG(LogTemp , Warning , TEXT("PlayerPawn을 찾지 못했습니다."));
    //    }
    //}
}





// JBS 추가
void AK_CesiumTeleportBox::NotifyFlight(APawn* flightPawn)
{
    // 해당 폰의 pc 가져오기
    auto* pc = flightPawn->GetController<AJ_MissionPlayerController>();
    if(!pc) return;
    
    pc->SRPC_AddFlightArySelf();
}

void AK_CesiumTeleportBox::SRPC_ChangeMissionArea_Implementation()
{
    const FVector targetVec(DestLatitude, DestLongitude, 0);

    MRPC_ChangeMissionArea(targetVec);
}

void AK_CesiumTeleportBox::MRPC_ChangeMissionArea_Implementation(const FVector& location)
{
    // 지구
    auto* geoRef = ACesiumGeoreference::GetDefaultGeoreference(this);
    
    // 좌표 이동
    geoRef->SetOriginLongitudeLatitudeHeight(location);
}