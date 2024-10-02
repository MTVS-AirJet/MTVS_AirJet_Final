// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <Http.h>
#include "J_JsonTemp.generated.h"

UCLASS()
class MTVS_AIRJET_FINAL_API AJ_JsonTemp : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AJ_JsonTemp();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:


public:

protected:
    void ReqPost(const FString &url = "");

    void ResPost(FHttpRequestPtr req, FHttpResponsePtr res, bool isSuccess);

public:
};
