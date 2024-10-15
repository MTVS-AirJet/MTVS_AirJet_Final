// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KHS/K_StreamingUI.h"
#include "J_StreamingUI.generated.h"

/**
 * 
 */
UCLASS()
class MTVS_AIRJET_FINAL_API UJ_StreamingUI : public UK_StreamingUI
{
	GENERATED_BODY()

	protected:
		UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Object")
		class AJ_BaseMissionPawn* gameObject;
			public:
		__declspec(property(get = GetGameObject, put = SetGameObject)) class AJ_BaseMissionPawn* GAME_OBJECT;
		class AJ_BaseMissionPawn* GetGameObject();
		void SetGameObject(class AJ_BaseMissionPawn* value)
		{
			gameObject = value;
		}
			protected:

	public:
	
	protected:
		virtual void NativeConstruct() override;
		virtual void NativeOnInitialized() override;
		// 화면 공유 버튼 클릭
		virtual void OnButtonWindowScreen() override;
                // 
		virtual void OnButtonLookSharingScreen() override;
		// 주어진 ID를 사용하여 StreamingActor의 공유 사용자 ID 및 화면 공유 설정
		virtual void SetUserID(FString ID, const bool &bAddPlayer) override;

            public:
	
};
