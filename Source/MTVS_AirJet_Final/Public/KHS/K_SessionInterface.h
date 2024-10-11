// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "K_SessionInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UK_SessionInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class MTVS_AIRJET_FINAL_API IK_SessionInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	
	virtual void Host(FString ServerName) = 0;  //방생성 가상함수
	virtual void Join(uint32 Index) = 0;  //방합류 가상함수
	virtual void TravelMainLobbyMap(bool bKeepCurrentSound) = 0;  //위젯레벨 로드 가상함수
	virtual void RefreshServerList() = 0;  //서버목록갱신 가상함수

};
