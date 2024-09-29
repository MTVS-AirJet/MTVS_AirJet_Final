// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

// AIRJET_LOG 이름의 로그 카테고리를 Warning 단계로 정의
DECLARE_LOG_CATEGORY_EXTERN(AIRJET_LOG, Log, All);

#define LOCALROLE (UEnum::GetValueAsString<ENetRole>(GetLocalRole()))
#define REMOTEROLE (UEnum::GetValueAsString<ENetRole>(GetRemoteRole()))

#define NETMODE (GetWorld()->GetNetMode()==ENetMode::NM_Client ? TEXT("Client") : GetWorld()->GetNetMode()==NM_Standalone ? TEXT("Standalone") : TEXT("Server"))

#define CALLINFO (FString(__FUNCTION__) + TEXT("(") + FString::FromInt(__LINE__) + TEXT(")"))

// NetMode, 소스 라인번호, 로그 내용 출력
#define LOG_S(Verbosity, fmt, ...) UE_LOG(AIRJET_LOG, Verbosity, TEXT("[%s]%s : %s"), NETMODE, *CALLINFO, *FString::Printf(fmt, ##__VA_ARGS__));
// Screen에다가 LOG를 직접 찍는 매크로
#define	LOG_SCREEN(fmt, ...) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT(fmt), ##__VA_ARGS__))
// 위의 함수에 시간을 넣는 매크로.
#define	LOG_SCREEN_T(Time, fmt, ...) GEngine->AddOnScreenDebugMessage(-1, Time, FColor::Red, FString::Printf(TEXT(fmt), ##__VA_ARGS__))
