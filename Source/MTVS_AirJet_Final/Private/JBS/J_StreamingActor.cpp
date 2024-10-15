// Fill out your copyright notice in the Description page of Project Settings.


#include "JBS/J_StreamingActor.h"
#include "Camera/CameraComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/Engine.h"
#include "GameFramework/Pawn.h"
#include "JBS/J_BaseMissionPawn.h"
#include "JBS/J_Utility.h"
#include "KHS/K_GameState.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Materials/MaterialInstanceDynamic.h"

void AJ_StreamingActor::BeginPlay()
{
    Super::BeginPlay();


}

void AJ_StreamingActor::Tick(float DeltaTime)
{
    // 최초 플레이어 카메라 설정
    if(!isInitScreen)
    {
        isInitScreen = TryInitScreen();
    }
    else {
        
    }

    Super::Tick(DeltaTime);
}

bool AJ_StreamingActor::TryInitScreen()
{
    // 로컬 플레이어 찾기
    APawn* playerPawn = UGameplayStatics::GetPlayerPawn(GetWorld() , 0);
    if(!playerPawn) return false;

    UCameraComponent* playerCamera = playerPawn->GetComponentByClass<UCameraComponent>();
    if(!playerCamera) return false;
    
    // 플레이어 카메라에 자신 붙이기
    StreamingScreen->SetRelativeScale3D(FVector(3 , 2 , 1));
    sceneComp->AttachToComponent(playerCamera , FAttachmentTransformRules::SnapToTargetIncludingScale);

    // Z 축이 카메라를 향하도록 회전
	DynamicMaterial = UMaterialInstanceDynamic::Create(StreamingScreen->GetMaterial(0) , this);
	StreamingScreen->SetMaterial(0 , DynamicMaterial);
	StreamingScreen->SetRelativeLocationAndRotation(FVector(400 , 0 , 0) , FRotator(0 , 90 , 90));

    
    auto gs = Cast<AK_GameState>(GetWorld()->GetGameState());
    check(gs)

    GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString::Printf(TEXT("Current Streaming Player Num : %d"), gs->ArrStreamingUserID.Num()));
    // LOG_S(Warning , TEXT("Current Streaming Player Num : %d") , gs->ArrStreamingUserID.Num());

    //SceneCapture2D 초기화 한번더
    if ( !(RenderTarget && SceneCapture) )
    {
        UE_LOG(LogTemp , Error , TEXT("Initialization failed in BeginPlay"));

        return false;
    }

    SceneCapture->TextureTarget = RenderTarget;

    return true;
}

void AJ_StreamingActor::SetViewSharingUserID(FString ID, const bool &bAddPlayer)
{
    GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("5. SA 에서 공유 아이디 설정"));

    UserID = ID; //UserID에 스트리머 ID 

    //플레이어를 통해 Game State에 접근하여 Streaming 중인 userID배열을 전달
    auto* localPlayer = UJ_Utility::GetBaseMissionPawn(GetWorld(), 0);
    check(localPlayer)
    localPlayer->ServerRPC_SetStreamingPlayer(ID, bAddPlayer);

	// LOG_S(Warning , TEXT("Set Streaming Player ID : %s") , *ID);
}
