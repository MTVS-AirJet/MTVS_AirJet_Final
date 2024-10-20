// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <Http.h>
#include <JBS/J_Utility.h>
#include "JsonObjectConverter.h"
#include "J_JsonManager.generated.h"

// DECLARE_DELEGATE_TwoParams(FResponseDelegate, const FString&, bool);

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
	// @@ 임시 요청 데이터
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
	FSignupReq tempSignup;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
	FLoginReq tempLogin;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
	FString tempReqMapName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
	FMissionDataRes tempGetMissionData;

public:

protected:
#pragma region 웹 통신 테스트
	UFUNCTION(BlueprintCallable)
	void ReqTemp();

        // 0. 회원가입 요청 예시
	UFUNCTION(BlueprintCallable)
	void ReqSignup();

	UFUNCTION(BlueprintCallable)
	void ReqLogin();

	// @@ 5. 테스트용 로그인 했을때 반환 데이터 받기
	UFUNCTION(BlueprintCallable)
	void OnLoginData(const FLoginRes &resData);

	UFUNCTION(BlueprintCallable)
	void ReqTempAuth();

	// @@ 테스트용 로그인 인증 데이터 받기
	UFUNCTION(BlueprintCallable)
	void OnLoginAuthData(const FResSimple &resData);

	UFUNCTION(BlueprintCallable)
	void ReqGetMissionData();

	UFUNCTION(BlueprintCallable)
	void OnGetMissionData(const FMissionDataRes &resData);

#pragma endregion
    public:
};
