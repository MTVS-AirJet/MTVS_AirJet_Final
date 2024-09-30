// Fill out your copyright notice in the Description page of Project Settings.


#include "KHS/K_StreamingActor.h"
#include "KHS/K_StreamingUI.h"  
// Windows 관련 헤더 파일 포함
#include "Windows/AllowWindowsPlatformTypes.h"  // Unreal Engine에서 Windows 플랫폼 관련 타입을 허용
#include <Windows.h>  // Windows API를 사용하기 위한 Windows 헤더 파일을 포함
#include "Windows/HideWindowsPlatformTypes.h"  // Unreal Engine에서 Windows 플랫폼 관련 타입을 숨김
// 화면 뷰어 관련 헤더 파일
//#include "../../../../Plugins/Media/PixelStreaming/Source/PixelStreaming/Public/IPixelStreamingStreamer.h"  // 픽셀 스트리밍 스트리머 인터페이스 헤더
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
//#include "MetaRealmGameState.h"  // MetaRealm 게임 상태에 대한 헤더 
//#include "NetGameInstance.h"  // 네트워크 게임 인스턴스에 대한 헤더
//#include "PlayerCharacter.h"  // 플레이어 캐릭터에 대한 Unreal Engine 헤더 
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
    WindowScreenPlaneMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WindowScreenPlaneMesh"));  
    WindowScreenPlaneMesh->SetupAttachment(sceneComp);  
    WindowScreenPlaneMesh->SetRelativeLocation(FVector(0, 0, 0));  
    WindowScreenPlaneMesh->SetRelativeScale3D(FVector(10.0, 10.0, 1.0));  

    // 기본 Plane Mesh 설정
    static ConstructorHelpers::FObjectFinder<UStaticMesh> PlaneMeshAsset(
        TEXT("/Script/Engine.StaticMesh'/Engine/BasicShapes/Plane.Plane'"));  
    if (PlaneMeshAsset.Succeeded())  
    {
        WindowScreenPlaneMesh->SetStaticMesh(PlaneMeshAsset.Object);  // 메쉬를 설정합니다.
    }

    // 기본 머티리얼 설정
    static ConstructorHelpers::FObjectFinder<UMaterial> DefaultMaterial(TEXT(
        "/Script/Engine.Material'/Game/Blueprints/KHS/PixelStreaming/MT_PixelStreaming_Mat.MT_PixelStreaming_Mat'"));  // 기본 머티리얼을 찾습니다.
    if (DefaultMaterial.Succeeded())  // 머티리얼 로드에 성공했을 경우
    {
        WindowScreenPlaneMesh->SetMaterial(0, DefaultMaterial.Object);  // 머티리얼을 설정합니다.
    }
    WindowScreenPlaneMesh->SetVisibility(false);  // 기본적으로 화면을 보이지 않게 설정합니다.

    // RenderTarget 초기화 (화면을 캡처할 때 사용)
    RenderTarget = CreateDefaultSubobject<UTextureRenderTarget2D>(TEXT("RenderTarget"));  // RenderTarget 객체를 생성합니다.
    static ConstructorHelpers::FObjectFinder<UTextureRenderTarget2D> RenderTargetAsset(
        TEXT("/Script/Engine.TextureRenderTarget2D'/Game/Blueprints/KHS/PixelStreaming/RT_PixelStreaming.RT_PixelStreaming'"));  // RenderTarget을 찾습니다.
    if (RenderTargetAsset.Succeeded())  // 로드에 성공하면
    {
        RenderTarget = RenderTargetAsset.Object;  // RenderTarget을 설정합니다.
    }

    RenderTarget->CompressionSettings = TextureCompressionSettings::TC_Default;  // 텍스처 압축 설정을 기본으로 설정합니다.
    RenderTarget->SRGB = false;  // sRGB 모드를 비활성화합니다.
    RenderTarget->bAutoGenerateMips = false;  // 자동으로 MipMap을 생성하지 않도록 설정합니다.
    RenderTarget->bForceLinearGamma = true;  // 선형 감마를 강제로 적용합니다.
    RenderTarget->TargetGamma = 2.2f;  // 감마 값을 설정합니다.
    RenderTarget->AddressX = TextureAddress::TA_Clamp;  // 텍스처의 X축 주소를 클램프 모드로 설정합니다.
    RenderTarget->AddressY = TextureAddress::TA_Clamp;  // 텍스처의 Y축 주소를 클램프 모드로 설정합니다.
    RenderTarget->InitAutoFormat(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));  // 시스템 화면 해상도에 맞춰 RenderTarget을 초기화합니다.

    // Scene Capture Component (2D) 초기화 (장면을 캡처하여 텍스처로 변환)
    SceneCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCapture"));  // SceneCaptureComponent2D를 생성합니다.
    SceneCapture->SetupAttachment(RootComponent);  // SceneCapture를 루트 컴포넌트에 부착합니다.
    SceneCapture->SetRelativeLocation(FVector(-1170.0, 0, 0));  // SceneCapture의 위치를 설정합니다.
    SceneCapture->CaptureSource = SCS_FinalColorLDR;  // 캡처 소스를 최종 컬러 LDR로 설정합니다.
    SceneCapture->TextureTarget = RenderTarget;  // 캡처한 내용을 RenderTarget에 저장합니다.

    // 초기에는 텍스처 업데이트를 하지 않도록 설정
    bShouldUpdateTexture = false;  // 텍스처를 업데이트할지 여부를 결정하는 플래그를 false로 설정합니다

}

// Called when the game starts or when spawned
void AK_StreamingActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AK_StreamingActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// 화면을 읽어오는 함수 (캡처된 화면을 처리)
void AK_StreamingActor::ReadFrame()
{
}

// OpenCV의 Mat 형식을 Unreal Engine의 UTexture2D로 변환하는 함수
UTexture2D* AK_StreamingActor::MatToTexture2D(const cv::Mat InMat)
{
	return nullptr;
}

// 모니터 전체 화면을 캡처하여 OpenCV의 Mat 객체로 반환하는 함수
cv::Mat AK_StreamingActor::GetScreenToCVMat()
{
	return cv::Mat();
}

// 특정 창의 화면을 캡처하여 OpenCV의 Mat 객체로 반환하는 함수
cv::Mat AK_StreamingActor::GetWindowToCVMat(HWND hwnd)
{
	return cv::Mat();
}

// 활성 창의 타이틀을 출력하는 함수
void AK_StreamingActor::LogActiveWindowTitles()
{
}

// 주어진 창 제목에 해당하는 창을 찾는 함수
void AK_StreamingActor::FindTargetWindow(FString TargetWindowTitle)
{
}

// 매 프레임 텍스처를 업데이트하는 함수
void AK_StreamingActor::UpdateTexture()
{
}

// 주어진 사용자 ID를 설정하여 화면 공유를 시작하는 함수
void AK_StreamingActor::SetViewSharingUserID(FString ID, const bool& bAddPlayer)
{
}

// 화면 공유를 중지하는 함수
void AK_StreamingActor::StopLookSharingScreen()
{
}

// 스트리밍을 시작하는 함수
void AK_StreamingActor::BeginStreaming()
{
}

// 화면 공유를 시작하는 함수
void AK_StreamingActor::BeginLookSharingScreen()
{
}

// 화면 공유를 다른 사용자 시점으로 변경하는 함수
void AK_StreamingActor::ChangeLookSharingScreen()
{
}

