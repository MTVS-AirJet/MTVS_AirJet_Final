// Fill out your copyright notice in the Description page of Project Settings.


#include "KHS/K_StreamingActor.h"
#include "KHS/K_StreamingUI.h"  
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
        WindowScreenPlaneMesh->SetStaticMesh(PlaneMeshAsset.Object);  
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
    if (RenderTargetAsset.Succeeded())  
    {
        RenderTarget = RenderTargetAsset.Object;  
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
	
    APawn* playerPawn = UGameplayStatics::GetPlayerPawn(GetWorld() , 0);
    UCameraComponent* playerCamera = playerPawn->GetComponentByClass<UCameraComponent>();
    WindowScreenPlaneMesh->SetRelativeScale3D(FVector(3 , 2 , 1));
    sceneComp->AttachToComponent(playerCamera , FAttachmentTransformRules::SnapToTargetIncludingScale); //카메라 붙이기

    // Z 축이 카메라를 향하도록 회전
    DynamicMaterial = UMaterialInstanceDynamic::Create(WindowScreenPlaneMesh->GetMaterial(0) , this);
    WindowScreenPlaneMesh->SetMaterial(0 , DynamicMaterial);
    WindowScreenPlaneMesh->SetRelativeLocationAndRotation(FVector(400 , 0 , 0) , FRotator(0 , 90 , 90));

    //GameState클래스 만들고나서 다시 살리기
    /*gs = Cast<AMetaRealmGameState>(GetWorld()->GetGameState());
    if ( gs )
    {
        AB_LOG(LogABNetwork , Log , TEXT("======================================================================"));
        AB_LOG(LogABNetwork , Log , TEXT("Current Streaming Player Num : %d") , gs->ArrStreamingUserID.Num());
    }*/

    if ( RenderTarget && SceneCapture )
    {
        SceneCapture->TextureTarget = RenderTarget;
    }
    else
    {
        UE_LOG(LogTemp , Error , TEXT("Initialization failed in BeginPlay"));
    }

    // 활성 창의 타이틀을 출력하는 함수
    LogActiveWindowTitles();

}

// Called every frame
void AK_StreamingActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    static float TimeAccumulator = 0.0f;
    const float CaptureInterval = 0.5f; // 0.5초마다 텍스처 업데이트
    TimeAccumulator += DeltaTime;

    if ( TimeAccumulator >= CaptureInterval && bShouldUpdateTexture )
    {
        TimeAccumulator = 0.0f;
        UpdateTexture();  // 일정 시간 간격으로만 업데이트
    }

}

// 화면을 읽어오는 함수 (캡처된 화면을 처리)
void AK_StreamingActor::ReadFrame()
{
    cv::Mat desktopImage = GetScreenToCVMat();
    imageTexture = MatToTexture2D(desktopImage);

    // 캡처된 화면을 동적 머티리얼에 적용
    if ( DynamicMaterial && imageTexture && WindowScreenPlaneMesh )
    {
        DynamicMaterial->SetTextureParameterValue(TEXT("Base") , imageTexture);
    }
}

// OpenCV의 Mat 형식을 Unreal Engine의 UTexture2D로 변환하는 함수
UTexture2D* AK_StreamingActor::MatToTexture2D(const cv::Mat InMat)
{
    UTexture2D* Texture = UTexture2D::CreateTransient(InMat.cols , InMat.rows , PF_B8G8R8A8);

    if ( InMat.type() == CV_8UC3 )
    {
        cv::Mat bgraImage;
        cv::cvtColor(InMat , bgraImage , cv::COLOR_BGR2BGRA);

        FTexture2DMipMap& Mip = Texture->GetPlatformData()->Mips[0];
        void* Data = Mip.BulkData.Lock(LOCK_READ_WRITE);
        FMemory::Memcpy(Data , bgraImage.data , bgraImage.total() * bgraImage.elemSize());
        Mip.BulkData.Unlock();
        Texture->PostEditChange();

        Texture->UpdateResource();
        return Texture;
    }
    else if ( InMat.type() == CV_8UC4 )
    {
        FTexture2DMipMap& Mip = Texture->GetPlatformData()->Mips[0];
        void* Data = Mip.BulkData.Lock(LOCK_READ_WRITE);
        FMemory::Memcpy(Data , InMat.data , InMat.total() * InMat.elemSize());
        Mip.BulkData.Unlock();
        Texture->PostEditChange();
        Texture->UpdateResource();
        return Texture;
    }

    Texture->PostEditChange();
    Texture->UpdateResource();
    return Texture;
}

// 모니터 전체 화면을 캡처하여 OpenCV의 Mat 객체로 반환하는 함수
cv::Mat AK_StreamingActor::GetScreenToCVMat()
{
    HDC hScreenDC = GetDC(NULL);
    HDC hMemoryDC = CreateCompatibleDC(hScreenDC);
    int screenWidth = GetDeviceCaps(hScreenDC , HORZRES);
    int screenHeight = GetDeviceCaps(hScreenDC , VERTRES);

    HBITMAP hBitmap = CreateCompatibleBitmap(hScreenDC , screenWidth , screenHeight);
    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemoryDC , hBitmap);
    BitBlt(hMemoryDC , 0 , 0 , screenWidth , screenHeight , hScreenDC , 0 , 0 , SRCCOPY);
    SelectObject(hMemoryDC , hOldBitmap);

    cv::Mat matImage(screenHeight , screenWidth , CV_8UC4);
    GetBitmapBits(hBitmap , matImage.total() * matImage.elemSize() , matImage.data);

    return matImage;
}

// 특정 창의 화면을 캡처하여 OpenCV의 Mat 객체로 반환하는 함수
cv::Mat AK_StreamingActor::GetWindowToCVMat(HWND hwnd)
{
    RECT windowRect;
    GetWindowRect(hwnd , &windowRect);

    int windowWidth = windowRect.right - windowRect.left;
    int windowHeight = windowRect.bottom - windowRect.top;

    HDC hWindowDC = GetDC(hwnd);
    HDC hMemoryDC = CreateCompatibleDC(hWindowDC);

    HBITMAP hBitmap = CreateCompatibleBitmap(hWindowDC , windowWidth , windowHeight);
    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemoryDC , hBitmap);

    BitBlt(hMemoryDC , 0 , 0 , windowWidth , windowHeight , hWindowDC , 0 , 0 , SRCCOPY);
    SelectObject(hMemoryDC , hOldBitmap);

    cv::Mat windowImage(windowHeight , windowWidth , CV_8UC4);
    GetBitmapBits(hBitmap , windowImage.total() * windowImage.elemSize() , windowImage.data);

    ReleaseDC(hwnd , hWindowDC);
    DeleteDC(hMemoryDC);
    DeleteObject(hBitmap);

    return windowImage;
}

// 활성 창의 타이틀을 출력하는 함수
void AK_StreamingActor::LogActiveWindowTitles()
{
    WindowTitles.Empty();

    EnumWindows([](HWND hwnd , LPARAM lParam) -> BOOL
        {
            int length = GetWindowTextLength(hwnd);
            if ( length == 0 )
                return true;

            if ( !IsWindowVisible(hwnd) )
                return true;

            WINDOWPLACEMENT placement;
            placement.length = sizeof(WINDOWPLACEMENT);
            GetWindowPlacement(hwnd , &placement);
            if ( placement.showCmd == SW_SHOWMINIMIZED )
                return true;

            LONG style = GetWindowLong(hwnd , GWL_STYLE);
            if ( !(style & WS_OVERLAPPEDWINDOW) )
                return true;

            TCHAR windowTitle[256];
            GetWindowText(hwnd , windowTitle , 256);

            TArray<FString>* WindowList = (TArray<FString>*)lParam;
            WindowList->Add(FString(windowTitle));

            FString title = FString(windowTitle);
            UE_LOG(LogTemp , Log , TEXT("Active Window: %s") , *title);

            return true;
        } , (LPARAM)&WindowTitles);
}

// 주어진 창 제목에 해당하는 창을 찾는 함수
void AK_StreamingActor::FindTargetWindow(FString TargetWindowTitle)
{
    TargetWindowHandle = nullptr;

    EnumWindows([](HWND hwnd , LPARAM lParam) -> BOOL
        {
            TCHAR windowTitle[256];
            GetWindowText(hwnd , windowTitle , 256);

            // 타겟을 찾음
            if ( FString(windowTitle) == *(FString*)lParam )
            {
                HWND* targetHandle = (HWND*)lParam;
                *targetHandle = hwnd;
                return false;
            }

            return true; //타겟을 찾지 못했을 경우 계속 순회
        } , (LPARAM)&TargetWindowHandle);

    if ( TargetWindowHandle == nullptr )
    {
        UE_LOG(LogTemp , Warning , TEXT("Target window not found"));
        bShouldUpdateTexture = false;
    }
    else
    {
        //UE_LOG(LogTemp, Log, TEXT("Target window found: "));
        bShouldUpdateTexture = true;
    }
}

// 매 프레임 텍스처를 업데이트하는 함수
void AK_StreamingActor::UpdateTexture()
{
    if ( TargetWindowHandle != nullptr )
    {
        //특정 앱만 찾아서 화면 공유
        cv::Mat windowImage = GetWindowToCVMat(TargetWindowHandle);
        imageTexture = MatToTexture2D(windowImage);
        //UE_LOG(LogTemp, Warning, TEXT("Successfully captured the window: ChatGPT - Chrome"));
    }
    else
    {
        //UE_LOG(LogTemp, Warning, TEXT("Target window not found. Capturing main screen instead."));
        ReadFrame();
    }

    //먼저 GetWorld()가 유효한지 확인합니다.
    if ( !GetWorld() )
    {
        UE_LOG(LogTemp , Error , TEXT("GetWorld() is null!"));
        return;
    }

    if ( DynamicMaterial && imageTexture && WindowScreenPlaneMesh )
    {
        // BaseTexture 파라미터에 텍스처 설정
        DynamicMaterial->SetTextureParameterValue(TEXT("Base") , imageTexture);
    }
}

// 주어진 사용자 ID를 설정하여 화면 공유를 시작하는 함수
void AK_StreamingActor::SetViewSharingUserID(FString ID, const bool& bAddPlayer)
{
    UserID = ID; //UserID에 스트리머 ID 

    /*if ( auto Mycharacter = Cast<APlayerCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn()) )
        Mycharacter->ServerRPC_SetStreamingPlayer(ID , bAddPlayer);

    AB_LOG(LogABNetwork , Log , TEXT("Set Streaming Player ID : %s") , *ID);*/
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

// 스트리밍을 시작하는 함수
void AK_StreamingActor::BeginStreaming()
{
    // 블루프린트 함수 이름
    FName FunctionName(TEXT("BeginStreaming")); // 블루프린트에서 정의한 함수명

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
        FString userID = "Editor";
        ProcessEvent(Function , &userID);
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
        FString userID = "Editor";
        // 블루프린트 함수 호출 (매개변수가 없는 경우)
        ProcessEvent(Function , &userID);
    }
    else
    {
        UE_LOG(LogTemp , Warning , TEXT("Function not found: %s") , *FunctionName.ToString());
    }
}

