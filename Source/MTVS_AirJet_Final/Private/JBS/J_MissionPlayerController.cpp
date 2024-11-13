// Fill out your copyright notice in the Description page of Project Settings.


#include "JBS/J_MissionPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Components/AudioComponent.h"
#include "Components/SlateWrapperTypes.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GenericPlatform/ICursor.h"
#include "JBS/J_BaseMissionPawn.h"
#include "JBS/J_JsonUtility.h"
#include "JBS/J_ObjectiveUIComp.h"
#include "KHS/K_LoadingWidget.h"
#include "Kismet/GameplayStatics.h"
#include "LHJ/L_Viper.h"
#include "Math/MathFwd.h"
#include <KHS/K_GameInstance.h>
#include <JBS/J_Utility.h>
#include <JBS/J_MissionGamemode.h>
#include "KHS/K_StreamingUI.h"
#include "Net/UnrealNetwork.h"
#include "Sound/SoundBase.h"
#include "Sound/SoundWave.h"
#include "Sound/SoundWaveProcedural.h"
#include "Templates/Casts.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"
#include "UObject/UObjectGlobals.h"

AJ_MissionPlayerController::AJ_MissionPlayerController()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryActorTick.bCanEverTick = true;


    commanderAudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("commanderAudioComp"));
    commanderAudioComp->SetupAttachment(RootComponent);
}

void AJ_MissionPlayerController::BeginPlay()
{
    Super::BeginPlay();

    // objuiComp 가져오기
    objUIComp = GetComponentByClass<UJ_ObjectiveUIComp>();
}



void AJ_MissionPlayerController::Tick(float deltaTime)
{
    Super::Tick(deltaTime);

    // GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString::Printf(TEXT("현재 플레이어 역할 : %s"), *UEnum::GetValueAsString(playerRole)));

    // GEngine->AddOnScreenDebugMessage(-1, -1.f, FColor::Green, FString::Printf(TEXT("현재 입력 모드 : %s"), *GetCurrentInputModeDebugString()));

    // auto* player = this->GetPawn();
    // GEngine->AddOnScreenDebugMessage(-1, -1.f, FColor::Green, FString::Printf(TEXT("포제스 중인 폰 이름 : %s"), player ? *player->GetName() : TEXT("폰 없음")));

    // GEngine->AddOnScreenDebugMessage(-1, -1.f, FColor::Green, FString::Printf(TEXT("this -> %s"), *UEnum::GetValueAsString(this->pilotRole)));
}

// 미션 게임모드에서 역할 설정 후 실행됨.
void AJ_MissionPlayerController::SpawnMyPlayer()
{
    // solved gi에서 내 역할을 가져오고
    // 해당 역할에 맞는 플레이어 생성 후 포제스
    // if(this->IsLocalController())
    {
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
    // 자기 역할에 맞는 스폰 위치 가져오기
    auto* gm = UJ_Utility::GetMissionGamemode(GetWorld());
    FTransform spawnTR = gm->GetPlayerSpawnTransfrom(playerRole, this);
    // 항상 생성
    FActorSpawnParameters SpawnParams;
    // Always spawn, regardless of whether there are other actors at that location
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    auto* player = GetWorld()->SpawnActor<AJ_BaseMissionPawn>(playerPrefab, spawnTR, SpawnParams);
    check(player);
    // 클라에서 포제스
    // CRPC_SpawnMyPlayer(player);
    this->Possess(player);
}

void AJ_MissionPlayerController::CRPC_SpawnMyPlayer_Implementation(APawn *newPawn)
{
    // this->Possess(newPawn);
}

void AJ_MissionPlayerController::OnPossess(APawn *newPawn)
{
    Super::OnPossess(newPawn);
    // 서버 단임
    // 로컬 클라에서 포제스시 작동
    if(HasAuthority())
    {
        FTimerHandle timerHandle;
        GetWorld()->GetTimerManager()
            .SetTimer(timerHandle, [this,newPawn]() mutable
        {
            //타이머에서 할 거
            // UE_LOG(LogTemp, Warning, TEXT("야 : %s"), *this->GetName());
            CRPC_OnPossess();    
        }, .5f, false);
        
    }
}

void AJ_MissionPlayerController::CRPC_OnPossess_Implementation()
{
    // 폰 가져오기
    auto* pilot = GetPawn<AL_Viper>();
    // 포제스 시 시동 절차 수행 딜리게이트 바인드
    pilot->engineProgSuccessDel.BindUObject(this, &AJ_MissionPlayerController::SRPC_SendEngineProgressSuccess);
    
    // UE_LOG(LogTemp, Warning, TEXT("아기 바인드 pc : %s, 폰 있음 : %s"), *this->GetName(), *UJ_Utility::ToStringBool(pilot != nullptr));
}

// XXX 이제 안씀
void AJ_MissionPlayerController::InitStreamingUI(AJ_BaseMissionPawn* newPawn)
{
	this->StreamingUI = CastChecked<UK_StreamingUI>(CreateWidget(GetWorld() , this->StreamingUIFactory));
	if ( this->StreamingUI )
	{
		// GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("2. StreaingUI is not null"));
		UE_LOG(LogTemp , Warning , TEXT("StreaingUI is not null"));

        newPawn->streamingUI = this->StreamingUI;

		StreamingUI->AddToViewport(0);
		StreamingUI->SetVisibility(ESlateVisibility::Visible);

		// pc->SetInputMode(FInputModeGameOnly()); //트래블할때 문제가 있어서 UI불러올떄 GameOnly로 설정
		//블루프린트로 Tab키 바인딩해서 필요할때 마우스 껐다켰다하면서 UI사용하면됨.
	}
	else
	{
		// GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("2-2. streamingUI is null"));
		UE_LOG(LogTemp , Warning , TEXT("streamingUI is null"));
	}
}

void AJ_MissionPlayerController::SRPC_AddFlightArySelf_Implementation()
{
    if(HasAuthority())
    {
        // 이륙한 pc 배열에 추가
        auto* gm = UJ_Utility::GetMissionGamemode(GetWorld());
        check(gm);
        gm->AddFlightedPC(this);
    }
}   

void AJ_MissionPlayerController::CRPC_AddLoadingUI_Implementation()
{
    // 로딩 ui 생성
    missionReadyUI = CreateWidget<UK_LoadingWidget>(GetWorld(), loadingUIPrefab);
    // 뷰포트에 추가
    missionReadyUI->AddToViewport();
}

void AJ_MissionPlayerController::CRPC_RemoveLoadingUI_Implementation()
{
    GetWorld()->GetTimerManager()
        .SetTimer(removeLoadingUITH, [this]() mutable
    {
        //타이머에서 할 거
        if(missionReadyUI)
        {
            missionReadyUI->RemoveFromParent();
            missionReadyUI = nullptr;
        }
        // ui 제거했으면 타이머 종료
        else {
            GetWorld()->GetTimerManager().ClearTimer(removeLoadingUITH);
        }
    }, 1.f, true);
}

void AJ_MissionPlayerController::SRPC_RemoveLoadingUI_Implementation()
{
    // UE_LOG(LogTemp, Warning, TEXT("asd나한테 왜그러는거니 : %s"), *this->GetName());
    CRPC_RemoveLoadingUI();
}

void AJ_MissionPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AJ_MissionPlayerController, pilotRole);
}

void AJ_MissionPlayerController::MRPC_TeleportStartPoint_Implementation(FTransform tpTR)
{
    // 전송
    auto* pawn = this->GetPawn();
    pawn->SetActorTransform(tpTR);
    auto* viper = Cast<AL_Viper>(pawn);
    viper->QuatCurrentRotation = tpTR.GetRotation();
    
}

void AJ_MissionPlayerController::SRPC_SendEngineProgressSuccess_Implementation(EEngineProgress type)
{
    GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("시동 절차 진행"));
    
    sendEngineProgDel.ExecuteIfBound(this, type);
}

void AJ_MissionPlayerController::CRPC_PlayCommanderVoice_Implementation(const FString &voiceBase64)
{
    // 디코딩
    auto* voice = UJ_JsonUtility::ConvertBase64WavToSound(voiceBase64);
    if(!voice) return;
    // 정지
    commanderAudioComp->Stop();
    // 소리 설정
    commanderAudioComp->SetSound(voice);
    // 재생
    commanderAudioComp->Play();
}
