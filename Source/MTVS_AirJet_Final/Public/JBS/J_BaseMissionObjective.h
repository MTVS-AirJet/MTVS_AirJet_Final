// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "CoreMinimal.h"
#include "J_Utility.h"
#include "GameFramework/Actor.h"

#include "J_BaseMissionObjective.generated.h"


DECLARE_MULTICAST_DELEGATE(FObjectiveEndDelegate)
DECLARE_MULTICAST_DELEGATE(FObjectiveActiveDelegate)

UCLASS()
class MTVS_AIRJET_FINAL_API AJ_BaseMissionObjective : public AActor
{
	GENERATED_BODY()
public:	
	// Sets default values for this actor's properties
	AJ_BaseMissionObjective();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Component")
	class USceneComponent* rootComp;

	// world UI 아이콘 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Component")
	class UWidgetComponent* iconWorldUIComp;
	// 아이콘 UI
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|UI")
	class UUserWidget* iconWorldUI;


	// 전술명령 활성화 여부
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
	bool isObjectiveActive = false;
		public:
	__declspec(property(get = GetObjectiveActive, put = SetObjectiveActive)) bool IS_OBJECTIVE_ACTIVE;
	virtual bool GetObjectiveActive()
	{
		return isObjectiveActive;
	}
	virtual void SetObjectiveActive(bool value);

protected:
public:
	// 명령 종류
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
	ETacticalOrder orderType = ETacticalOrder::NONE;

	// 미션 완료 시 실행 딜리게이트
	FObjectiveEndDelegate objectiveEndDel;
	// 미션 성공 시 실행 딜리게이트
	FObjectiveEndDelegate objectiveSuccessDel;
	// 미션 실패 시 실행 딜리게이트
	FObjectiveEndDelegate objectiveFailDel;

	// 미션 활성화 시 실행 딜리게이트
	FObjectiveActiveDelegate objectiveActiveDel;

	// 미션 비활성화 시 실행 딜리게이트
	FObjectiveActiveDelegate objectiveDeactiveDel;

protected:
	// 목표 성공/실패
	UFUNCTION(BlueprintCallable)
    virtual void ObjectiveSuccess();
	UFUNCTION(BlueprintCallable)
    virtual void ObjectiveFail();

	// 목표 활성화/비활성화
	UFUNCTION(BlueprintCallable)
	virtual void ObjectiveActive();
	UFUNCTION(BlueprintCallable)
	virtual void ObjectiveDeactive();

    public:
	// 미션 종료 처리
	UFUNCTION(BlueprintCallable)
	virtual void ObjectiveEnd(bool isSuccess = true);

	// 목표 값 설정
	virtual void InitObjective(ETacticalOrder type, bool initActive = false);
};
