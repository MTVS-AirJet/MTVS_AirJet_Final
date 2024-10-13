// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/UniformGridPanel.h"  // UniformGridPanel UI 컴포넌트를 포함하는 헤더 파일
#include "Components/Button.h"  // 버튼 컴포넌트에 대한 헤더 파일
#include "Components/TextBlock.h"  // 텍스트 블록 컴포넌트에 대한 헤더 파일
#include "K_StreamingUI.generated.h"

/**
 * 
 */
UCLASS()
class MTVS_AIRJET_FINAL_API UK_StreamingUI : public UUserWidget
{
	GENERATED_BODY()
	
public:
	
    virtual void NativeConstruct() override;  
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);  
    virtual void NativeOnInitialized();  
    virtual void NativeDestruct();  

    //==========================================================================
    //  Function
    //==========================================================================
    
    // 주어진 ID를 사용하여 StreamingActor의 공유 사용자 ID 및 화면 공유 설정
    UFUNCTION()  
    virtual void SetUserID(FString ID, const bool& bAddPlayer);  

    // 화면 공유 버튼을 눌렀을 때, 호출될 델리게이트에 등록할 함수(조종사 Plane메쉬)
    UFUNCTION(BlueprintCallable)  
    virtual void OnButtonWindowScreen();  

    // 다른 사용자의 화면을 볼 때 호출되는 함수(지휘관 UI에)
    UFUNCTION(BlueprintCallable)  
    virtual void OnButtonLookSharingScreen();  

    // StreamingActor를 설정하는 함수
    virtual void SetScreenActor(class AK_StreamingActor* Actor);

    // 현재 온라인 세션의 ID를 반환하는 함수
    virtual FString GetCurrentSessionID();  

    // 사용자 ID 목록을 받아 여러 사용자 슬롯을 초기화하는 함수
    virtual void InitSlot(TArray<FString> Items);  
    

    //==========================================================================
    //  Variables, Instance
    //==========================================================================
public:
    // UMG에서 가져올 GridPanel, 창 목록 버튼, 및 관련된 위젯 바인딩
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))  
    class UUniformGridPanel* WindowList;  // 스트리머 버튼 목록을 표시할 위젯
    UPROPERTY(VisibleAnywhere, meta = (BindWidget)) 
    class UButton* ButtonWindowScreen;  // 화면 공유 시작 버튼 
    UPROPERTY(VisibleAnywhere, meta = (BindWidget))  
    class UButton* ButtonLookSharingScreen;  // 다른 사용자의 화면보기 버튼 
    UPROPERTY(meta = (BindWidget))  
    class UTextBlock* TextWindowScreen;  // ButtonWindowScreen의 상태 텍스트 블록 ("Screen Share", "Sharing")
    UPROPERTY(meta = (BindWidget))  
    class UTextBlock* TextLookSharingScreen;  // ButtonLookSharingScreen의 상태 텍스트 블록 ("Screen Look", "Watching")
    UPROPERTY(meta = (BindWidget)) 
    class UImage* ImageSharingScreen;  // 공유된 화면을 표시할 이미지 컴포넌트
    

    UPROPERTY(EditAnywhere, BlueprintReadWrite)  
    TSubclassOf<UUserWidget> SharingUserSlotFactory;  // 창 목록에서 사용자 정보를 표시할 위젯 템플릿
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)  
    class UK_SharingUserSlot* SharingUserSlot;  // 각 사용자 정보를 표시할 슬롯

    UPROPERTY()  
    class AK_StreamingActor* ScreenActor;  // 화면 공유와 관련된 ScreenActor 액터

    bool bStreaming;  // 현재 화면을 공유 중인지 여부를 나타내는 플래그
    bool bLookStreaming;  // 현재 화면 공유를 보는 중인지 여부를 나타내는 플래그

    TSharedPtr<class IPixelStreamingStreamer> CurrentStreamer;  // 현재의 화면 공유 스트리머를 나타내는 포인터

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Capture", meta = (AllowPrivateAccess = "true"))  
    class USceneCaptureComponent2D* SceneCapture;  // 화면을 캡처하는 장면 캡처 컴포넌트
    
    TSharedPtr<class FPixelStreamingVideoInputRenderTarget> VideoInput; // 화면 캡처 결과를 저장할 렌더 타겟
   
private:
    UPROPERTY()  
    class AMTVS_AirJet_FinalCharacter* Me;  // 현재 플레이어 캐릭터를 나타내는 포인터

};
