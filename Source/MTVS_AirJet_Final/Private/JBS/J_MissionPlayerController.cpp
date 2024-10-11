// Fill out your copyright notice in the Description page of Project Settings.


#include "JBS/J_MissionPlayerController.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GenericPlatform/ICursor.h"
#include "Kismet/GameplayStatics.h"
#include "Math/MathFwd.h"
#include <KHS/K_GameInstance.h>
#include <JBS/J_Utility.h>
#include <JBS/J_MissionGamemode.h>

void AJ_MissionPlayerController::BeginPlay()
{
    Super::BeginPlay();

    // solved 임시 spawnpos 가져오기
    // TArray<AActor*> outActors;
    // UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), AActor::StaticClass(), FName(TEXT("SpawnPos")), outActors);
    // spawnTR = outActors[0]->GetActorTransform();

    
}

void AJ_MissionPlayerController::Tick(float deltaTime)
{
    Super::Tick(deltaTime);

    // GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString::Printf(TEXT("현재 플레이어 역할 : %s"), *UEnum::GetValueAsString(playerRole)));

}

// 미션 게임모드에서 역할 설정 후 실행됨.
void AJ_MissionPlayerController::SpawnMyPlayer()
{
    // GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("11"));
    // solved gi에서 내 역할을 가져오고
    // 해당 역할에 맞는 플레이어 생성 후 포제스
    // if(this->IsLocalController())
    {
        // GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("22"));
        auto* gi = UJ_Utility::GetKGameInstance(GetWorld());
        check(gi);
        playerRole = gi->PLAYER_ROLE;

        // 플레이어 생성
        auto prefab = gi->GetMissionPlayerPrefab();
        SRPC_SpawnMyPlayer(prefab);

        // 지휘관은 커서 보이게 처리
        switch (playerRole)
        {
            case EPlayerRole::COMMANDER:
                this->bShowMouseCursor = true;
                DefaultMouseCursor = EMouseCursor::Crosshairs;
                SetInputMode(FInputModeGameAndUI());
                
                break;
            case EPlayerRole::PILOT:
                this->bShowMouseCursor = false;
                DefaultMouseCursor = EMouseCursor::Crosshairs;
                SetInputMode(FInputModeGameOnly());
                break;
        }
    }
}

void AJ_MissionPlayerController::SRPC_SpawnMyPlayer_Implementation(TSubclassOf<class APawn> playerPrefab)
{
    // FIXME spawnTR 게임모드에서 역할에 맞는 위치 가져올 수 있도록 해야할 듯
    auto* gm = UJ_Utility::GetMissionGamemode(GetWorld());
    FTransform spawnTR = gm->GetPlayerSpawnTransfrom(playerRole);
    // 항상 생성
    FActorSpawnParameters SpawnParams;
    // Always spawn, regardless of whether there are other actors at that location
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    auto* player = GetWorld()->SpawnActor<APawn>(playerPrefab, spawnTR, SpawnParams);
    
    this->Possess(player);
}