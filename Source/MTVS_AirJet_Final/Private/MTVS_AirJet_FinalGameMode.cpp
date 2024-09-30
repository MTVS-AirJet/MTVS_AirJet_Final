// Copyright Epic Games, Inc. All Rights Reserved.

#include "MTVS_AirJet_FinalGameMode.h"
#include "MTVS_AirJet_FinalCharacter.h"
#include "UObject/ConstructorHelpers.h"

AMTVS_AirJet_FinalGameMode::AMTVS_AirJet_FinalGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
