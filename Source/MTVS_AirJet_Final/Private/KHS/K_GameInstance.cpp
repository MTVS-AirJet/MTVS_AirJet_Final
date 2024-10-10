// Fill out your copyright notice in the Description page of Project Settings.


#include "KHS/K_GameInstance.h"
#include "KHS/K_LoginRegisterWidget.h"

#include "JBS/J_JsonUtility.h"
#include "JBS/J_Utility.h"

#include "Components/AudioComponent.h"
#include <Engine/World.h>
#include "Engine/Engine.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "JsonObjectConverter.h"
#include "Math/UnrealMathUtility.h"
#include "Templates/SubclassOf.h"
#include "Kismet/GameplayStatics.h"

void UK_GameInstance::Host(FString ServerName)
{
}

void UK_GameInstance::Join(uint32 Index)
{
}

void UK_GameInstance::CreateSession()
{
}

void UK_GameInstance::RefreshServerList()
{
}

// 로그인 UI를 생성하는 함수
void UK_GameInstance::CreateLoginWidget()
{
	// LoginWidgetFactory를 통해 LogInUI 위젯 생성
	LoginWidget = CreateWidget<UK_LoginRegisterWidget>(this , LoginWidgetFactory);
	LoginWidget->SetInterface(this); //부모함수 호출
	LoginWidget->SetUI(); //부모함수 호출
}

void UK_GameInstance::LoadServerWidgetMap(bool bKeepCurrentSound)
{
}

// 로비 사운드 재생 함수
void UK_GameInstance::PlayLobbySound()
{
	if ( LobbySound )
	{
		if ( CurrentPlayingSound )
		{
			CurrentPlayingSound->Stop();
		}
		CurrentPlayingSound = UGameplayStatics::SpawnSound2D(this , LobbySound , 1.0f , 1.0f , 0.0f , nullptr , true , false);
		UE_LOG(LogTemp , Warning , TEXT("Started playing lobby sound"));
	}
	else
	{
		UE_LOG(LogTemp , Error , TEXT("LobbySound is not set"));
	}
}

// 현재 사운드 유지 함수(로그인시)
void UK_GameInstance::ContinueCurrentSound()
{
	// 현재 재생 중인 사운드가 있다면 계속 재생
	if ( CurrentPlayingSound && !CurrentPlayingSound->IsPlaying() )
	{
		CurrentPlayingSound->Play();
	}
	else if ( !CurrentPlayingSound )
	{
		// 현재 재생 중인 사운드가 없다면 로비 사운드 재생
		PlayLobbySound();
	}
}

#pragma region BackEnd Coordination
void UK_GameInstance::SetUserId(const FString& NewUserId)
{	
	UserId = NewUserId;
}

FString UK_GameInstance::GetUserId() const
{
	return UserId;
}
#pragma endregion



