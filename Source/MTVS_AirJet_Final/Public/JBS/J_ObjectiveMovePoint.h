// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "JBS/J_BaseMissionObjective.h"
#include "J_ObjectiveMovePoint.generated.h"

/**
 * 
 */
UCLASS()
class MTVS_AIRJET_FINAL_API AJ_ObjectiveMovePoint : public AJ_BaseMissionObjective
{
	GENERATED_BODY()
public:	
	// Sets default values for this actor's properties
	AJ_ObjectiveMovePoint();

protected:
	// 성공 판정 충돌체
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Component")
	class UCapsuleComponent* checkCapsuleComp;

	// 빔 길이
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
	float beamLength = 500000.f;

	// 빔 충돌 판정 폭
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
	float beamRadius = 500.f;

	// FIXME 임시 타이머 핸들
	FTimerHandle timerHandle;

	// 실패 판정할 기준 방향 | 목표 <- 편대장 의 방향
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Values")
	FVector baseDirection;

	// 실패 판정할 거리 | 일단 100m
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
	float failDis = 10000.f;

public:

protected:
	virtual void BeginPlay();

	// 빔 이펙트 생성
    void InitBeamVFX();

	// 성공 판정 충돌
	UFUNCTION()
	virtual void OnCheckCapsuleBeginOverlap(
		UPrimitiveComponent *OverlappedComponent, AActor *OtherActor,
		UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep,
		const FHitResult &SweepResult);

	// 목표 완료 시 비활성화 처리
	virtual void ObjectiveEnd(bool isSuccess = true) override;

	// 실패 체크 | 기준 방향 보다 현재 상대 위치가 넘어가면 실패
	bool CheckFail(const FVector &baseDir);

	virtual void ObjectiveActive();

    public:
        virtual void Tick(float deltaTime) override;
        virtual void SetObjectiveActive(bool value) override;
};
