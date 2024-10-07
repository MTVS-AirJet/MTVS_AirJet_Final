// Fill out your copyright notice in the Description page of Project Settings.


#include "JBS/J_JsonManager.h"
#include "Engine/Engine.h"
#include "JBS/J_Utility.h"
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
	auto* gi = UJ_Utility::GetJGameInstance(GetWorld());
	gi->tempLoginAuthUseDel.BindUObject(this, &AJ_JsonManager::OnLoginAuthData);

	UJ_JsonUtility::RequestExecute<FLogin>(GetWorld(), EJsonType::TEMP02_AUTH, tempLogin);
}

void AJ_JsonManager::OnLoginAuthData(const FResSimple &resData)
{
	// GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("asd"));
	GEngine->AddOnScreenDebugMessage(-1, 31.f, FColor::Yellow, FString::Printf(TEXT("jsonManager에서 출력됨\n%s"), *resData.ToString()));
}
