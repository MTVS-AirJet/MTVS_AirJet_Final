// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "K_SharingUserSlot.generated.h"

/**
 * 
 */
UCLASS()
class MTVS_AIRJET_FINAL_API UK_SharingUserSlot : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* UserIDText;  // UserID를 표시하는 텍스트 블록

    UPROPERTY(meta = (BindWidget))
    class UButton* UserIDButton;  // 사용자 ID 버튼

    // UserID를 설정하는 함수
    UFUNCTION(BlueprintCallable)
    void SetUserID(FString ID);

    // 해당 사용자에 대한 ID 저장
    FString CurrentUserID;

    // 사용자 ID 버튼 클릭 시 호출될 함수
    UFUNCTION()
    void OnUserIDButtonClicked();

    //스크린 액터
    UPROPERTY(EditAnywhere , BlueprintReadWrite)
    class AK_StreamingActor* StreamingActor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UUserWidget> StreamingUIFactory;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UK_StreamingUI* StreamingUI; //사용자 정보를 표시하는 슬롯
};
