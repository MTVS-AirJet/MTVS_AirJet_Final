// Fill out your copyright notice in the Description page of Project Settings.


#include "JBS/J_MissionGameState.h"
#include "Containers/Array.h"
#include "Containers/UnrealString.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "JBS/J_BaseMissionPawn.h"
#include "JBS/J_MissionPlayerController.h"
#include "JBS/J_Utility.h"
#include "KHS/K_StreamingUI.h"
#include "Templates/Casts.h"
#include "UObject/ObjectPtr.h"

AJ_MissionGameState::AJ_MissionGameState()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AJ_MissionGameState::OnRep_StreamingID()
{
    // LOG_S(Warning , TEXT("ArrStreamingUserID Num : %d") , ArrStreamingUserID.Num());

	// SharingSlot위젯에 방송중인 플레이어 리스트 업데이트
    // solved 로컬 플레이어 찾아서 UI 에 공유 플레이어 목록 생성 | ui 의존성 제거
    // auto* localPlayer = UJ_Utility::GetBaseMissionPawn(GetWorld());
    // check(localPlayer);
    // localPlayer->streamingUI->InitSlot(ArrStreamingUserID);
}

void AJ_MissionGameState::AddStreamUserId(const FString &userId)
{
    // 이미 있는지 확인
    if(this->ArrStreamingUserID.Contains(userId)) return;

    ArrStreamingUserID.Add(userId);
}

void AJ_MissionGameState::RemoveStreamUserId(const FString &userId)
{
    // 요소 제거
    ArrStreamingUserID.Remove(userId);
}

TArray<AJ_MissionPlayerController *> AJ_MissionGameState::GetAllPlayerController()
{
    if(!HasAuthority()) return TArray<AJ_MissionPlayerController*>();

    TArray<AJ_MissionPlayerController*> allPC;
    Algo::Transform(this->PlayerArray, allPC, [](TObjectPtr<APlayerState> temp){
        check(temp);
        auto* tempPC0 = temp->GetPlayerController();
        check(tempPC0);
        auto* tempPC = CastChecked<AJ_MissionPlayerController>(tempPC0);
        check(tempPC);

        return tempPC;
    });

    return allPC;
}

TArray<APawn *> AJ_MissionGameState::GetAllPlayerPawn()
{
    if(!HasAuthority()) return TArray<APawn*>();

    // 플레이어 스테이트->PS->폰 으로 가져오기
    // 모든 pc
    auto allPC = GetAllPlayerController();

    // pc -> apawn 변환
    TArray<APawn*> allPawns;
    Algo::Transform(allPC, allPawns, [](AJ_MissionPlayerController* tempPC){
        auto* tempPawn = tempPC->GetPawn();
        check(tempPawn);

        return tempPawn;
    });
    
    return allPawns;
}

void AJ_MissionGameState::RemoveAllLoadingUI()
{
    if(!HasAuthority()) return;

    auto allPC = GetAllPlayerController();
    for(auto* pc : allPC)
    {
        pc->CRPC_RemoveLoadingUI();
    }
}

void AJ_MissionGameState::Tick(float deltaTime)
{
    Super::Tick(deltaTime);

    // if(!HasAuthority()) return;

    // TArray<AJ_MissionPlayerController*> allPC;
    // Algo::Transform(this->PlayerArray, allPC, [](TObjectPtr<APlayerState> temp){
    //     auto* tempPC = Cast<AJ_MissionPlayerController>(temp->GetPlayerController());
    //     return tempPC;
    // });

    // FString a1 = FString::Printf(TEXT("pc1 : %s"), allPC[0] ? TEXT("있어") : TEXT("없어"));
    
    // GEngine->AddOnScreenDebugMessage(-1, -1.f, FColor::Green, FString::Printf(TEXT("%s 권한 : %s")
    // , *a1
    // , *UJ_Utility::ToStringBool(HasAuthority())));
}