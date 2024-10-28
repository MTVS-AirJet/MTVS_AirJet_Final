// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "K_CesiumTeleportBox.generated.h"

UCLASS()
class MTVS_AIRJET_FINAL_API AK_CesiumTeleportBox : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AK_CesiumTeleportBox();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly)
	class UBoxComponent* BoxComp;

	UPROPERTY(EditDefaultsOnly)
	class UStaticMeshComponent* MeshComp;

	//Component기반 충돌감지
	UFUNCTION()
	void OnMyBoxBeginOverlap( UPrimitiveComponent* OverlappedComponent , AActor* OtherActor , UPrimitiveComponent* OtherComp , int32 OtherBodyIndex , bool bFromSweep , const FHitResult& SweepResult );
	
	//충돌한 플레이어와 월드 폴리곤을 옮기기
	//위경도 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	double DestLongitude;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	double DestLatitude;

	class ACesiumGeoreference* WorldGeoReference;

	//인스턴스에서 가지고 있던 위경도 조건을 가져오는 함수
	UFUNCTION()
	void SetDestinationLogitudeLatitude(double Logitude, double Latitude);

	//저장된 변수를 입력받아서 플레이어와 월드폴리곤위치를 옮기는 함수
	void MovePlayerandPolygonToDestination(double Longitude, double Latitude);

};
