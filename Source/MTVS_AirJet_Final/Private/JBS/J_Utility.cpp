// Fill out your copyright notice in the Description page of Project Settings.


#include "JBS/J_Utility.h"
#include "Kismet/GameplayStatics.h"
#include <JBS/J_GameInstance.h>
#include <Engine/World.h>

UJ_GameInstance *UJ_Utility::GetJGameInstance(const UWorld* world)
{
    auto* gi =  Cast<UJ_GameInstance>(UGameplayStatics::GetGameInstance(world));
    check(gi);

    return gi;
}

