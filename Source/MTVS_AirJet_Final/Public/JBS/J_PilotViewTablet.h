// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "J_PilotViewTablet.generated.h"

UCLASS()
class MTVS_AIRJET_FINAL_API AJ_PilotViewTablet : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AJ_PilotViewTablet();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Component")
	class USceneComponent* tabletRoot;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Component")
	class UWidgetComponent* tabletUIComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|UI")
	class UJ_PilotViewTabletUI* tabletUI; 

	// 디버그용 로컬 rt 사용
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Debug")
	bool enableLocalScreenShare = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Debug")
	class UTextureRenderTarget* debugRT;

public:

protected:
    void StartViewScreenShare();

public:
};
