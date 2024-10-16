// Fill out your copyright notice in the Description page of Project Settings.


#include "KHS/K_JsonParseLib.h"
#include "KHS/K_GameState.h"
#include "KHS/K_GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "JBS/J_BaseMissionPawn.h"
#include "JBS/J_MissionGamemode.h"
#include "JBS/J_GameInstance.h"

UJ_GameInstance* UK_JsonParseLib::GetJGameInstance(const UWorld* world)
{
	auto* gi = Cast<UJ_GameInstance>(UGameplayStatics::GetGameInstance(world));
	check(gi);

	return gi;
}

UK_GameInstance* UK_JsonParseLib::GetKGameInstance(const UWorld* world)
{
	auto* gi = Cast<UK_GameInstance>(UGameplayStatics::GetGameInstance(world));
	check(gi);

	return gi;
}

AJ_MissionGamemode* UK_JsonParseLib::GetMissionGamemode(const UWorld* world)
{
	auto* gm = world->GetAuthGameMode<AJ_MissionGamemode>();
	check(gm);

	return gm;
}

AK_GameState* UK_JsonParseLib::GetKGameState(const UWorld* world)
{
	auto* gs = world->GetGameState<AK_GameState>();
	check(gs);

	return gs;
}

AJ_BaseMissionPawn* UK_JsonParseLib::GetBaseMissionPawn(const UWorld* world , int32 playerIdx)
{
	auto* player = Cast<AJ_BaseMissionPawn>(UGameplayStatics::GetPlayerController(world , playerIdx));
	check(player);

	return player;
}

