// Copyright Epic Games, Inc. All Rights Reserved.

#include "MTVS_AirJet_FinalGameMode.h"
#include "MTVS_AirJet_FinalCharacter.h"
#include "UObject/ConstructorHelpers.h"

AMTVS_AirJet_FinalGameMode::AMTVS_AirJet_FinalGameMode()
{
	// set default pawn class to our Blueprinted character
	//ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter_C"));
	// auto a = PlayerPawnBPClass.;
	// if (PlayerPawnBPClass.Succeeded())
	// {
	// 	DefaultPawnClass = PlayerPawnBPClass.Object;
	// }
}
