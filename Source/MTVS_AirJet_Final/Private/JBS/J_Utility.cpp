// Fill out your copyright notice in the Description page of Project Settings.


#include "JBS/J_Utility.h"
#include "JBS/J_BaseMissionPawn.h"
#include "JBS/J_MissionGameState.h"
#include "KHS/K_GameState.h"
#include "Kismet/GameplayStatics.h"
#include "Math/MathFwd.h"
#include <JBS/J_GameInstance.h>
#include <KHS/K_GameInstance.h>
#include <JBS/J_MissionGamemode.h>
#include <Engine/World.h>

UJ_GameInstance *UJ_Utility::GetJGameInstance(const UWorld* world)
{
    auto* gi =  Cast<UJ_GameInstance>(UGameplayStatics::GetGameInstance(world));
    check(gi);

    return gi;
}

UK_GameInstance *UJ_Utility::GetKGameInstance(const UWorld* world)
{
    auto* gi =  Cast<UK_GameInstance>(UGameplayStatics::GetGameInstance(world));
    check(gi);

    return gi;
}

AJ_MissionGamemode* UJ_Utility::GetMissionGamemode(const UWorld* world)
{
    auto* gm = world->GetAuthGameMode<AJ_MissionGamemode>();
    check(gm);

    return gm;
}

AK_GameState *UJ_Utility::GetKGameState(const UWorld *world)
{
    auto* gs = world->GetGameState<AK_GameState>();
    check(gs);

    return gs;
}

AJ_MissionGameState* UJ_Utility::GetMissionGameState(const UWorld* world)
{
    auto* gs = world->GetGameState<AJ_MissionGameState>();
    check(gs);

    return gs;
}

AJ_BaseMissionPawn *UJ_Utility::GetBaseMissionPawn(const UWorld *world, int32 playerIdx)
{
    auto* pc = UGameplayStatics::GetPlayerController(world, playerIdx);
    check(pc);
    auto* player = pc->GetPawn<AJ_BaseMissionPawn>();
    check(player);

    return player;
}

// === 구조체 함수 구현

ETacticalOrder FMissionObject::GetOrderType() const
{
    ETacticalOrder type;

    switch(this->commandNo)
    {
        case 0: type = ETacticalOrder::MOVE_THIS_POINT; break;
        case 1: type = ETacticalOrder::FORMATION_FLIGHT; break;
        case 2: type = ETacticalOrder::NEUTRALIZE_TARGET; break;

        default: type = ETacticalOrder::NONE; break;
    }

    return type;
}

FTransform FJVector2D::GetTransform() const
{
    // x,y 값 이랑 정해진 고도 값을 가지고 변환
    // 범위 == 5킬로 기준 -25만 ~ 25만
    float minValue = UJ_Utility::defaultMissionMapSize * -.5f;
    float maxValue = UJ_Utility::defaultMissionMapSize * .5f;
    // x,y,z 계산
    float mapX = FMath::Lerp(minValue, maxValue, this->x / 100.f);
    float mapY = FMath::Lerp(minValue, maxValue, this->y / 100.f);
    float mapZ = UJ_Utility::defaultMissionObjectHeight;

    FVector vec = FVector(mapX, mapY, mapZ);
    
    // @@ 회전 값 받도록 요청해야 할 지도?
    FTransform tr = FTransform(FRotator::ZeroRotator,vec,  FVector::OneVector);

    return tr;
}
