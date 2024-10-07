// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <Http.h>
#include <JBS/J_Utility.h>
#include "JsonObjectConverter.h"
#include "J_JsonManager.generated.h"

DECLARE_DELEGATE_TwoParams(FResponseDelegate, const FString&, bool);

UCLASS()
class MTVS_AIRJET_FINAL_API AJ_JsonManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AJ_JsonManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// 기본 ip
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
	FString defaultURL = TEXT("http://125.132.216.190:7757/api/");
	// 인증값 // 로그인 시 받아옴
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Values")
	FString authorValue = TEXT("");

	// @@ 임시 요청 데이터
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
	FSignup tempSignup;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
	FLogin tempLogin;

	


public:

protected:
	// @@ 임시 요청 단
	// 나중엔 실제 요청하는 요소요소에 넣을것
	UFUNCTION(BlueprintCallable)
	void ReqSignup();

	UFUNCTION(BlueprintCallable)
	void ReqLogin();

	UFUNCTION(BlueprintCallable)
	void ReqTempAuth();

    public:


	
};
