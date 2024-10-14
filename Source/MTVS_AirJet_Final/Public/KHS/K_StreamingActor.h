// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Windows/AllowWindowsPlatformTypes.h"  // Unreal Engine에서 Windows 플랫폼 타입을 사용하기 위한 설정
#include <Windows.h>  // Windows API를 사용하기 위한 Windows 헤더 파일 포함
#include "Windows/HideWindowsPlatformTypes.h"  // Windows 플랫폼 타입을 숨기기 위한 설정

#include "PreOpenCVHeaders.h"  // OpenCV 라이브러리 사용 전 필요한 설정 헤더
#include <opencv2/opencv.hpp>  // OpenCV 라이브러리의 핵심 기능을 포함한 헤더
#include "PostOpenCVHeaders.h"  // OpenCV 라이브러리 사용 후 필요한 설정 헤더

#include "HAL/CriticalSection.h"  // 여러 스레드 간의 동기화를 위한 CriticalSection 클래스
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "K_StreamingActor.generated.h"

UCLASS()
class MTVS_AIRJET_FINAL_API AK_StreamingActor : public AActor
{
	GENERATED_BODY()

    // 다중 스레드에서 화면 캡처와 텍스처 업데이트 작업 시 데이터 경쟁을 방지하기 위한 CriticalSection
	FCriticalSection CriticalSection;  

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
    // Variables
    //==========================================================================

    UPROPERTY(BlueprintReadWrite, Category = "PixelStreaming")
	FString UserID; //픽셀 스트리밍 아이디

    TSharedPtr<class IPixelStreamingStreamer> CurrentStreamer;

    UPROPERTY(EditDefaultsOnly , BlueprintReadWrite , Category = "Components")
	class UStaticMeshComponent* StreamingScreen; //픽셀 스트리밍 하는 Plane

    UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UUserWidget> StreamingUIFactory;  // 창 목록을 표시할 UUserWidget 서브클래스

    UPROPERTY(BlueprintReadWrite) 
    class UK_StreamingUI* StreamingUI;  // 픽셀 스트리밍 화면을 표시할 UI 위젯 인스턴스

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Capture")  
    class UTextureRenderTarget2D* RenderTarget;  // 화면을 캡처하여 저장할 렌더 타겟

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Capture") 
    class USceneCaptureComponent2D* SceneCapture;  // 2D 장면 캡처 컴포넌트

    //FString userID;

	//UPROPERTY()
	//class AK_GameState* gs;

    cv::VideoCapture capture;  // OpenCV에서 비디오 캡처를 위한 객체
    cv::Mat image;  // OpenCV의 Mat 타입으로 화면 이미지를 저장할 객체

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UTexture2D* imageTexture;  // OpenCV에서 처리된 화면 이미지를 저장하는 텍스처

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")  
    class USceneComponent* sceneComp;  // 액터의 루트 컴포넌트 역할을 하는 SceneComponent

    class UMaterialInstanceDynamic* DynamicMaterial;  // 동적 머티리얼 인스턴스 (캡처된 화면을 표시할 머티리얼)
    
public:
    //==========================================================================
    // Function
    //==========================================================================
    
    virtual void UpdateTexture();  // 매 프레임 텍스처를 업데이트하는 함수

    UFUNCTION(BlueprintCallable, Category = "PixelStreaming")
	virtual void SetViewSharingUserID(FString ID, const bool& bAddPlayer);  // 주어진 사용자 ID를 설정하여 화면 공유를 시작하는 함수

    // 블루프린트에서 호출할 함수
    virtual void StopLookSharingScreen();  // 화면 공유를 중지하는 함수

    //void BeginStreaming();  // 스트리밍을 시작하는 함수

    virtual void BeginLookSharingScreen();  // 화면 공유를 시작하는 함수

    virtual void ChangeLookSharingScreen();  // 화면 공유를 다른 사용자 시점으로 변경하는 함수

};
