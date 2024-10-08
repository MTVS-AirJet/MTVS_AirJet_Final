// Fill out your copyright notice in the Description page of Project Settings.


#include "KHS/K_StreamingActor.h"
#include "KHS/K_StreamingUI.h"  
#include "KHS/K_GameState.h"  
#include "MTVS_AirJet_FinalCharacter.h"  // 플레이어 캐릭터 헤더 
#include <MTVS_AirJet_Final.h> //프로젝트 헤더
// Windows 관련 헤더 파일 포함
#include "Windows/AllowWindowsPlatformTypes.h"  // Unreal Engine에서 Windows 플랫폼 관련 타입을 허용
#include <Windows.h>  // Windows API를 사용하기 위한 Windows 헤더 파일을 포함
#include "Windows/HideWindowsPlatformTypes.h"  // Unreal Engine에서 Windows 플랫폼 관련 타입을 숨김
// 화면 뷰어 관련 헤더 파일
#include "../../../../Plugins/Media/PixelStreaming/Source/PixelStreaming/Public/IPixelStreamingStreamer.h"  // 픽셀 스트리밍 스트리머 인터페이스 헤더
#include "Engine/TextureRenderTarget2D.h"  
#include "Engine/Texture2DDynamic.h"  // 동적 2D 텍스처 헤더 
#include "Engine/Texture2D.h"  
#include "Components/StaticMeshComponent.h"  
#include "Components/Image.h"  
#include "Components/SceneCaptureComponent2D.h" 
#include "Materials/MaterialInstanceDynamic.h"  // 동적 머티리얼 인스턴스 헤더 
#include <vector>  // C++ 표준 라이브러리 벡터 헤더
#include "Styling/SlateBrush.h"  // SlateBrush 헤더
#include "Rendering/Texture2DResource.h"  // 2D 텍스처 리소스 헤더 
#include "Kismet/KismetMathLibrary.h"  // Kismet의 수학 라이브러리 헤더 
#include "Kismet/GameplayStatics.h"  // 게임플레이 Kismet 관련 헤더 
#include "CanvasItem.h"  // 캔버스 아이템 관련 헤더 
#include "CanvasTypes.h"  // 캔버스 타입 관련 헤더 
#include "Camera/CameraComponent.h"  
#include "Net/UnrealNetwork.h"  // 네트워크 관련 헤더
//#include "NetGameInstance.h"  // 게임 인스턴스에 대한 헤더 
//#include "MetaRealm/MetaRealm.h"  // MetaRealm 관련된 헤더 


// Sets default values
AK_StreamingActor::AK_StreamingActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    // Scene Component 초기화
    sceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("sceneComp")); 
    SetRootComponent(sceneComp);  

    // Plane Mesh 초기화 (스크린 공유 시 화면을 표시할 평면)
    StreamingScreen = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WindowScreenPlaneMesh"));  
    StreamingScreen->SetupAttachment(sceneComp);  
    StreamingScreen->SetRelativeLocation(FVector(50, 0, 0));  
    StreamingScreen->SetRelativeScale3D(FVector(3.0, 3.0, 0.3));  

    // 기본 Plane Mesh 설정
    static ConstructorHelpers::FObjectFinder<UStaticMesh> PlaneMeshAsset(
        TEXT("/Script/Engine.StaticMesh'/Engine/BasicShapes/Plane.Plane'"));  
    if (PlaneMeshAsset.Succeeded())  
    {
        StreamingScreen->SetStaticMesh(PlaneMeshAsset.Object);  
    }

    // 기본 머티리얼 설정
    static ConstructorHelpers::FObjectFinder<UMaterial> DefaultMaterial(TEXT(
        "/Script/Engine.Material'/Game/Blueprints/KHS/PixelStreaming/MT_PixelStreaming_Mat.MT_PixelStreaming_Mat'")); 
    if (DefaultMaterial.Succeeded())  
    {
        StreamingScreen->SetMaterial(0, DefaultMaterial.Object); 
    }
    StreamingScreen->SetVisibility(false);  

    // RenderTarget 초기화 (화면을 캡처할 때 사용)
    RenderTarget = CreateDefaultSubobject<UTextureRenderTarget2D>(TEXT("RenderTarget"));  
    static ConstructorHelpers::FObjectFinder<UTextureRenderTarget2D> RenderTargetAsset(
        TEXT("/Script/Engine.TextureRenderTarget2D'/Game/Blueprints/KHS/PixelStreaming/RT_PixelStreaming.RT_PixelStreaming'"));  
    if (RenderTargetAsset.Succeeded())  
    {
        RenderTarget = RenderTargetAsset.Object;  
    }

    RenderTarget->CompressionSettings = TextureCompressionSettings::TC_Default;  // 텍스처 압축 설정을 기본으로 설정.
    RenderTarget->SRGB = false;  // sRGB 모드 비활성화
    RenderTarget->bAutoGenerateMips = false;  // 자동으로 MipMap을 생성하지 않도록 설정
    RenderTarget->bForceLinearGamma = true;  // 선형 감마 강제 적용
    RenderTarget->TargetGamma = 2.2f;  // 감마 값 설정
    RenderTarget->AddressX = TextureAddress::TA_Clamp;  // 텍스처의 X축 주소를 클램프 모드로 설정
    RenderTarget->AddressY = TextureAddress::TA_Clamp;  // 텍스처의 Y축 주소를 클램프 모드로 설정
    RenderTarget->InitAutoFormat(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));  // 시스템 화면 해상도에 맞춰 RenderTarget을 초기화

    // Scene Capture Component (2D) 초기화 (장면을 캡처하여 텍스처로 변환)
    SceneCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCapture"));  
    SceneCapture->SetupAttachment(RootComponent);  
    //SceneCapture->SetRelativeLocation(FVector(0, 0, 0));  // SceneCapture 위치 설정
    SceneCapture->CaptureSource = SCS_FinalColorLDR;  // 캡처 소스를 최종 컬러 LDR로 설정
    SceneCapture->TextureTarget = RenderTarget;  // 캡처한 내용을 RenderTarget에 저장

}

// Called when the game starts or when spawned
void AK_StreamingActor::BeginPlay()
{
	Super::BeginPlay();
	
    APawn* playerPawn = UGameplayStatics::GetPlayerPawn(GetWorld() , 0);
    UCameraComponent* playerCamera = playerPawn->GetComponentByClass<UCameraComponent>();
    StreamingScreen->SetRelativeScale3D(FVector(3 , 2 , 1));
    sceneComp->AttachToComponent(playerCamera , FAttachmentTransformRules::SnapToTargetIncludingScale); //카메라 붙이기

    // Z 축이 카메라를 향하도록 회전
	DynamicMaterial = UMaterialInstanceDynamic::Create(StreamingScreen->GetMaterial(0) , this);
	StreamingScreen->SetMaterial(0 , DynamicMaterial);
	StreamingScreen->SetRelativeLocationAndRotation(FVector(400 , 0 , 0) , FRotator(0 , 90 , 90));

    
    auto gs = Cast<AK_GameState>(GetWorld()->GetGameState());
    if ( gs )
    {
        LOG_S(Warning , TEXT("Current Streaming Player Num : %d") , gs->ArrStreamingUserID.Num());
    }

    //SceneCapture2D 초기화 한번더
    if ( RenderTarget && SceneCapture )
    {
        SceneCapture->TextureTarget = RenderTarget;
    }
    else
    {
        UE_LOG(LogTemp , Error , TEXT("Initialization failed in BeginPlay"));
    }
}

// Called every frame
void AK_StreamingActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    static float TimeAccumulator = 0.0f;
    const float CaptureInterval = 0.5f; // 0.5초마다 텍스처 업데이트
    TimeAccumulator += DeltaTime;

    if ( TimeAccumulator >= CaptureInterval)
    {
        TimeAccumulator = 0.0f;
        UpdateTexture();  // 일정 시간 간격으로만 업데이트
    }
}

// 매 프레임 텍스처를 업데이트하는 함수
void AK_StreamingActor::UpdateTexture()
{
    if ( DynamicMaterial && StreamingScreen )
    {
        // 동적 매터리얼의 BaseTexture 파라미터에 텍스처 설정
        DynamicMaterial->SetTextureParameterValue(TEXT("Base") , imageTexture);
    }
}

// 주어진 사용자 ID를 설정하여 화면 공유를 시작하는 함수
void AK_StreamingActor::SetViewSharingUserID(FString ID, const bool& bAddPlayer)
{
    UserID = ID; //UserID에 스트리머 ID 

    //플레이어를 통해 Game State에 접근하여 Streaming 중인 userID배열을 전달
	if ( auto MyCharacter = Cast<AMTVS_AirJet_FinalCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn()) )
		MyCharacter->ServerRPC_SetStreamingPlayer(ID , bAddPlayer);

	LOG_S(Warning , TEXT("Set Streaming Player ID : %s") , *ID);
}

// 화면 공유를 중지하는 함수
void AK_StreamingActor::StopLookSharingScreen()
{
    // 블루프린트 함수 이름
    FName FunctionName(TEXT("StopLookPixelStreaming")); // 블루프린트에서 정의한 함수명

    // 블루프린트 함수 가져오기
    UFunction* Function = FindFunction(FunctionName);

    if ( Function )
    {
        // 블루프린트 함수 호출 (매개변수가 없는 경우)
        ProcessEvent(Function , nullptr);
    }
    else
    {
        UE_LOG(LogTemp , Warning , TEXT("Function not found: %s") , *FunctionName.ToString());
    }
}

//// 스트리밍을 시작하는 함수
//void AK_StreamingActor::BeginStreaming()
//{
//    // 블루프린트 함수 이름
//    FName FunctionName(TEXT("BeginStreaming")); // 블루프린트에서 정의한 함수명
//
//    // 블루프린트 함수 가져오기
//    UFunction* Function = FindFunction(FunctionName);
//
//    if ( Function )
//    {
//        // 블루프린트 함수 호출 (매개변수가 없는 경우)
//        ProcessEvent(Function , nullptr);
//    }
//    else
//    {
//        UE_LOG(LogTemp , Warning , TEXT("Function not found: %s") , *FunctionName.ToString());
//    }
//}

// 화면 공유를 시작하는 함수
void AK_StreamingActor::BeginLookSharingScreen()
{
    // 블루프린트 함수 이름
    FName FunctionName(TEXT("BeginLookPixelStreaming")); // 블루프린트에서 정의한 함수명

    // 블루프린트 함수 가져오기
    UFunction* Function = FindFunction(FunctionName);

    if ( Function )
    {
        // 블루프린트 함수 호출 (매개변수가 없는 경우)
        ProcessEvent(Function , &UserID);
    }
    else
    {
        UE_LOG(LogTemp , Warning , TEXT("Function not found: %s") , *FunctionName.ToString());
    }
}

// 화면 공유를 다른 사용자 시점으로 변경하는 함수
void AK_StreamingActor::ChangeLookSharingScreen()
{
    // 블루프린트 함수 이름
    FName FunctionName(TEXT("ChangePixelStreaming")); // 블루프린트에서 정의한 함수명

    // 블루프린트 함수 가져오기
    UFunction* Function = FindFunction(FunctionName);

    if ( Function )
    {  
        // 블루프린트 함수 호출 (매개변수가 없는 경우)
        ProcessEvent(Function, &UserID);
    }
    else
    {
        UE_LOG(LogTemp , Warning , TEXT("Function not found: %s") , *FunctionName.ToString());
    }
}

