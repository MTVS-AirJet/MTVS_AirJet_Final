// Fill out your copyright notice in the Description page of Project Settings.


#include "JBS/J_MissionGameState.h"
#include "JBS/J_BaseMissionPawn.h"
#include "JBS/J_Utility.h"
#include "KHS/K_StreamingUI.h"

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