// Fill out your copyright notice in the Description page of Project Settings.


#include "JBS/J_MissionGameState.h"
#include "Containers/Array.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "JBS/J_BaseMissionPawn.h"
#include "JBS/J_MissionPlayerController.h"
#include "JBS/J_Utility.h"
#include "KHS/K_StreamingUI.h"
#include "Templates/Casts.h"
#include "UObject/ObjectPtr.h"

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
    TArray<AJ_MissionPlayerController*> allPC;
    Algo::Transform(this->PlayerArray, allPC, [](TObjectPtr<APlayerState> temp){
        check(temp);
        auto* tempPC = CastChecked<AJ_MissionPlayerController>(temp->GetPlayerController());
        check(tempPC);

        return tempPC;
    });

    return allPC;
}

TArray<APawn *> AJ_MissionGameState::GetAllPlayerPawn()
{
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