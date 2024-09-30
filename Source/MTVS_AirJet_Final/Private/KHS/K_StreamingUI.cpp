// Fill out your copyright notice in the Description page of Project Settings.


#include "KHS/K_StreamingUI.h"

// 위젯이 생성된 후 초기화할 때 호출되는 함수
void UK_StreamingUI::NativeConstruct()
{
}

// 매 프레임마다 호출되는 함수
void UK_StreamingUI::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
}

// 위젯이 처음으로 초기화될 때 호출되는 함수
void UK_StreamingUI::NativeOnInitialized()
{
}

// 현재 실행 중인 창에 대한 프로세스 목록을 그리드 패널에 추가하는 함수
void UK_StreamingUI::NativeDestruct()
{
}

// 주어진 ID를 사용하여 StreamingActor의 공유 사용자 ID를 설정
void UK_StreamingUI::SetUserID(FString ID, const bool& bAddPlayer)
{
}

// 화면 공유 버튼을 눌렀을 때, 호출될 델리게이트에 등록할 함수
void UK_StreamingUI::OnButtonWindowScreen()
{
}

// 다른 사용자의 화면을 볼 때 호출되는 함수
void UK_StreamingUI::OnButtonLookSharingScreen()
{
	
}

// StreamingActor를 설정하는 함수
void UK_StreamingUI::SetScreenActor(AScreenActor* Actor)
{
}

// 현재 온라인 세션의 ID를 반환하는 함수
FString UK_StreamingUI::GetCurrentSessionID()
{
	return FString();
}

// 사용자 ID 목록을 받아 여러 사용자 슬롯을 초기화하는 함수
void UK_StreamingUI::InitSlot(TArray<FString> Items)
{
	
}

// 현재 실행 중인 창에 대한 프로세스 목록을 그리드 패널에 추가하는 함수
void UK_StreamingUI::InitProcessListUI()
{
}
