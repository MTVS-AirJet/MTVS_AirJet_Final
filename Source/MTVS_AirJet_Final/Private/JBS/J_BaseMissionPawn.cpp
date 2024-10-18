// Fill out your copyright notice in the Description page of Project Settings.


#include "JBS/J_BaseMissionPawn.h"
#include "Blueprint/UserWidget.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Containers/UnrealString.h"
#include "Engine/Engine.h"
#include "IPixelStreamingModule.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "JBS/J_MissionPlayerController.h"
#include "JBS/J_MissionGameState.h"
#include <KHS/K_PlayerController.h>
#include <JBS/J_Utility.h>
#include "KHS/K_StreamingUI.h"
#include "KHS/K_StreamingActor.h"
#include "Modules/ModuleManager.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "PixelStreamingInputEnums.h"
#include "PixelStreamingVideoInputRenderTarget.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AJ_BaseMissionPawn::AJ_BaseMissionPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AJ_BaseMissionPawn::BeginPlay()
{
	Super::BeginPlay();

	// solved 플레이어가 포제스하면 화면 공유 시작
	if(this->IsLocallyControlled())
	{
		StartScreenShare();	
	}
}

void AJ_BaseMissionPawn::PossessedBy(AController *newController)
{
	Super::PossessedBy(newController);
	
	
}

// Called every frame
void AJ_BaseMissionPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AJ_BaseMissionPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AJ_BaseMissionPawn::ServerRPC_SetStreamingPlayer_Implementation(const FString &playerId, bool bAddPlayer)
{
	// k 게임 스테이트 가져오기
	auto* gs = UJ_Utility::GetMissionGameState(GetWorld());
	// userId 배열에 추가 or 제거
	if(bAddPlayer)
		gs->AddStreamUserId(playerId);
	else
		gs->RemoveStreamUserId(playerId);
	
	// XXX 동기화 | ui 보여줄거 아닌데 필요없는듯?
	// if(this->HasAuthority())
	// 	gs->OnRep_StreamingID();
}

// 화면 공유 시작 
void AJ_BaseMissionPawn::StartScreenShare()
{
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Purple, TEXT("0. 화면 공유 시작"));
	// 세션 아이디 가져오기
	streamId = GetSessionId();
	
	// 픽셀 스트리밍 모듈 활성화
	IPixelStreamingModule& psModule = FModuleManager::LoadModuleChecked<IPixelStreamingModule>(TEXT("PixelStreaming"));
	// 스트리밍 id 로 픽셀 스트리머 등록
	// 현재 화면 공유 스트리머
	auto curStreamer = psModule.CreateStreamer(streamId);
	if(!curStreamer) return;

	
	// solved SA의 현재 스트리머 변수에 자기 할당
	// STREAM_ACTOR->CurrentStreamer = curStreamer;
	STREAM_ACTOR->UserID = streamId;
	
	// 비디오 입력 처리 방법 ui 위젯 라우팅으로 설정
	curStreamer->SetInputHandlerType(EPixelStreamingInputType::RouteToWidget);
	// solved 캡쳐 활성화
	// STREAM_ACTOR->SceneCapture->Activate();

	// 화면 공유 플레이어 배열에 자신 추가
	ServerRPC_SetStreamingPlayer(streamId, true);

	// 비디오 입력 씬 캡처 렌더 타겟으로 설정
	auto videoInput = FPixelStreamingVideoInputRenderTarget::Create(this->STREAM_ACTOR->SceneCapture->TextureTarget);
	curStreamer->SetVideoInput(videoInput);
	// 수신용 url 설정
	curStreamer->SetSignallingServerURL(psServerURL);

	// 스트리밍 시작
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Purple, TEXT("2. 플레이어 화면 공유 시작"));
	curStreamer->StartStreaming();


	// @@ 생각해보니 SA 사실상 컴포넌트 처럼 작동하는데 아예 컴포넌트화 해도 되지않을까? 화면공유 컴포넌트 마냥, findorcreate해서 프리팹에 있으면 그걸로 하고 없으면 컴포넌트 생성하는 식으로
}

FString AJ_BaseMissionPawn::GetSessionId()
{
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Purple, TEXT("1. 세션 id 가져오기"));
	// UK_StreamingUI::GetCurrentSessionID() 인용
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if ( OnlineSubsystem )
	{
		IOnlineSessionPtr SessionInterface = OnlineSubsystem->GetSessionInterface();
		if ( SessionInterface.IsValid() )
		{
			// "GameSession"은 기본 세션 이름이며, 필요에 따라 다른 세션 이름을 사용할 수 있음
			FNamedOnlineSession* NamedSession = SessionInterface->GetNamedSession(NAME_GameSession);
			// 세션 ID 가져오기
			if ( NamedSession )
				return NamedSession->GetSessionIdStr();
		}
	}

	// 세션이 없거나 가져오지 못했을 때 -> 임시로 체크할때 이걸로 확인중
	return FString(TEXT("Local Player"));
}

AK_StreamingActor *AJ_BaseMissionPawn::GetStreamActor()
{
	// 없으면 맵에서 찾아오기
	// @@ 컴포넌트 화 할꺼면 자기 꺼에서 찾겠지
	if(!streamActor)
	{
		// GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("SA 찾아오기"));
		auto* tempSA = Cast<AK_StreamingActor>(
			UGameplayStatics::GetActorOfClass(GetWorld(), AK_StreamingActor::StaticClass()));
		if(tempSA)
			STREAM_ACTOR = tempSA;
	}

	return streamActor;
}