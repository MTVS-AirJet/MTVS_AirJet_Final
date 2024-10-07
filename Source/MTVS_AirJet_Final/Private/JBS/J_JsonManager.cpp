// Fill out your copyright notice in the Description page of Project Settings.


#include "JBS/J_JsonManager.h"
#include "Engine/Engine.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "JBS/J_JsonUtility.h"
#include "JBS/J_Utility.h"
#include "JsonObjectConverter.h"
#include "Math/UnrealMathUtility.h"
#include "Serialization/JsonTypes.h"
#include "Templates/SharedPointer.h"
#include "Templates/SharedPointerFwd.h"
#include <JBS/J_JsonUtility.h>

// Sets default values
AJ_JsonManager::AJ_JsonManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AJ_JsonManager::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AJ_JsonManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AJ_JsonManager::ReqSignup()
{
	UJ_JsonUtility::RequestExecute<FSignup>(GetWorld(), EJsonType::SIGN_UP, tempSignup);
}

void AJ_JsonManager::ReqLogin()
{
	UJ_JsonUtility::RequestExecute<FLogin>(GetWorld(), EJsonType::LOGIN, tempLogin);
}

void AJ_JsonManager::ReqTempAuth()
{
	UJ_JsonUtility::RequestExecute<FLogin>(GetWorld(), EJsonType::TEMP02_AUTH, tempLogin);
}