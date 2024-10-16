// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "K_JsonParseLib.h"
#include "K_ServerList.generated.h"

/**
 * 
 */
UCLASS()
class MTVS_AIRJET_FINAL_API UK_ServerList : public UUserWidget
{
	GENERATED_BODY()
	
public:
	
	// 위젯 바인딩 ================================================

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UButton* RowButton; // 모든 하위 위젯 개체를 그룹화한 버튼

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UTextBlock* sessionIdx; // 방 번호(세션)
	
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UTextBlock* SessionName; // 방 이름(세션)

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UTextBlock* HostName; // 세션 생성자명

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UTextBlock* ConnectedPlayer; // 접속자 수

	
	// Variables    ================================================

	UPROPERTY()
	class UK_ServerWidget* Parent;
	uint32 Index;

	
	UPROPERTY(BlueprintReadOnly)
	bool Selected = false; // 서버를 선택했는지, 아닌지 선언(Event Graph에서 사용)

	// Functions    ================================================
	void Setup(class UK_ServerWidget* Parent, uint32 Index);

	UFUNCTION()
	void OnHovered();

	UFUNCTION()
	void OnHoveredReq(const FMapInfoRequest& mapName);

	UFUNCTION()
	void OnClicked();

};
