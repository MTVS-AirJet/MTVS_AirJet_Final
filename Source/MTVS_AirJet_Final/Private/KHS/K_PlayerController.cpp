// Fill out your copyright notice in the Description page of Project Settings.


#include "KHS/K_PlayerController.h"

void AK_PlayerController::TravelToLobbyLevel()
{
	// 로비 맵으로 클라이언트를 이동
	ClientTravel("/Game/Maps/KHS/K_LobbyMap" , ETravelType::TRAVEL_Absolute);

}
