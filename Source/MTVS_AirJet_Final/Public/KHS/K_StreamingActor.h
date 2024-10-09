// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Windows/AllowWindowsPlatformTypes.h"  // Unreal Engine에서 Windows 플랫폼 타입을 사용하기 위한 설정
#include <Windows.h>  // Windows API를 사용하기 위한 Windows 헤더 파일 포함
#include "Windows/HideWindowsPlatformTypes.h"  // Windows 플랫폼 타입을 숨기기 위한 설정

#include "PreOpenCVHeaders.h"  // OpenCV 라이브러리 사용 전 필요한 설정 헤더 파일
#include <opencv2/opencv.hpp>  // OpenCV 라이브러리의 핵심 기능을 포함한 헤더 파일
#include "PostOpenCVHeaders.h"  // OpenCV 라이브러리 사용 후 필요한 설정 헤더 파일

#include "HAL/CriticalSection.h"  // 여러 스레드 간의 동기화를 위한 CriticalSection 클래스에 대한 헤더 파일
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "K_StreamingActor.generated.h"

UCLASS()
class MTVS_AIRJET_FINAL_API AK_StreamingActor : public AActor
{
	GENERATED_BODY()
	
	FCriticalSection CriticalSection;  // 다중 스레드에서 화면 캡처와 텍스처 업데이트 작업 시 데이터 경쟁을 방지하기 위한 CriticalSection

public:	
	// Sets default values for this actor's properties
	AK_StreamingActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:

    //==========================================================================
    // 화면 공유 관련 Function, Variables
    //==========================================================================

    UFUNCTION(BlueprintCallable) 
    void ReadFrame();  // 화면을 읽어오는 함수 (캡처된 화면을 처리)

    UTexture2D* MatToTexture2D(const cv::Mat InMat);  // OpenCV의 Mat 형식을 Unreal Engine의 UTexture2D로 변환하는 함수

    cv::Mat GetScreenToCVMat();  // 모니터 전체 화면을 캡처하여 OpenCV의 Mat 객체로 반환하는 함수
    cv::Mat GetWindowToCVMat(HWND hwnd);  // 특정 창의 화면을 캡처하여 OpenCV의 Mat 객체로 반환하는 함수

    void LogActiveWindowTitles();  // 활성 창의 타이틀을 출력하는 함수

    void FindTargetWindow(FString TargetWindowTitle);  // 주어진 창 제목에 해당하는 창을 찾는 함수

    void UpdateTexture();  // 매 프레임 텍스처를 업데이트하는 함수


    cv::VideoCapture capture;  // OpenCV에서 비디오 캡처를 위한 객체
    cv::Mat image;  // OpenCV의 Mat 타입으로 화면 이미지를 저장할 객체

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UTexture2D* imageTexture;  // OpenCV에서 처리된 화면 이미지를 저장하는 텍스처

    // 캡처할 특정 창의 핸들을 저장하는 변수
    HWND TargetWindowHandle;  // 특정 창의 핸들을 저장

    // 창 제목을 저장할 배열
    TArray<FString> WindowTitles;  // 활성 창 제목을 저장할 배열

    // 매 프레임 Tick에서 UpdateTexture를 호출할지 여부를 나타내는 플래그
    bool bShouldUpdateTexture;  // 텍스처 업데이트 여부를 나타내는 플래그

    // 액터의 전반적인 위치를 고정하기 위한 SceneComponent
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")  
    class USceneComponent* sceneComp;  // 액터의 루트 컴포넌트 역할을 하는 SceneComponent

    class UMaterialInstanceDynamic* DynamicMaterial;  // 동적 머티리얼 인스턴스 (캡처된 화면을 표시할 머티리얼)

    // UTexture2D* CaptureScreenToTexture();  // 화면을 캡처하여 텍스처로 변환하는 함수 (현재 주석 처리됨)
    // UTexture2D* CapturedTexture;  // 캡처된 텍스처 (현재 주석 처리됨)


    //==========================================================================
    // 픽셀스트리밍 관련 Function, Variables
    //==========================================================================
public:
    
    UFUNCTION(BlueprintCallable, Category = "PixelStreaming")
	void SetViewSharingUserID(FString ID, const bool& bAddPlayer);  // 주어진 사용자 ID를 설정하여 화면 공유를 시작하는 함수

    // 블루프린트에서 호출할 수 있는 함수 정의
    void StopLookSharingScreen();  // 화면 공유를 중지하는 함수

    void BeginStreaming();  // 스트리밍을 시작하는 함수

    void BeginLookSharingScreen();  // 화면 공유를 시작하는 함수

    void ChangeLookSharingScreen();  // 화면 공유를 다른 사용자 시점으로 변경하는 함수

    UPROPERTY(BlueprintReadWrite, Category = "PixelStreaming")
	FString UserID; //픽셀 스트리밍 아이디

    TSharedPtr<class IPixelStreamingStreamer> CurrentStreamer;

    UPROPERTY(EditDefaultsOnly , BlueprintReadWrite , Category = "Components")
	class UStaticMeshComponent* WindowScreenPlaneMesh; //픽셀 스트리밍 하는 Plane

    UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> WindowListFactory;  // 창 목록을 표시할 UUserWidget 서브클래스

    UPROPERTY(BlueprintReadWrite) 
    class UK_StreamingUI* WindowList;  // 픽셀 스트리밍 화면을 표시할 UI 위젯

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Capture")  
    class UTextureRenderTarget2D* RenderTarget;  // 화면을 캡처하여 저장할 렌더 타겟

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Capture") 
    class USceneCaptureComponent2D* SceneCapture;  // 2D 장면 캡처 컴포넌트

    //==========화면공유 추가
	//UPROPERTY()
	//class AMetaRealmGameState* gs;

};
