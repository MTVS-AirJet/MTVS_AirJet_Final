// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "JBS/J_BaseMissionObjective.h"
#include "J_MissionObjectiveMovePoint.generated.h"

/**
 * 
 */
UCLASS()
class MTVS_AIRJET_FINAL_API AJ_MissionObjectiveMovePoint : public AJ_BaseMissionObjective
{
	GENERATED_BODY()
public:	
	// Sets default values for this actor's properties
	AJ_MissionObjectiveMovePoint();

protected:
	// 성공 판정 충돌체
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Component")
	class UCapsuleComponent* checkCapsuleComp;

	// 빔 길이
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
	float beamLength = 500000.f;

	// FIXME 임시 타이머 핸들
	FTimerHandle timerHandle;

public:

protected:
	virtual void BeginPlay();

	// 빔 이펙트 생성
    void InitBeamVFX();

	// 성공 판정 충돌
	UFUNCTION()
	void OnCheckCapsuleBeginOverlap(
		UPrimitiveComponent *OverlappedComponent, AActor *OtherActor,
		UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep,
		const FHitResult &SweepResult);

	// 목표 완료 시 비활성화 처리
	virtual void ObjectiveEnd(bool isSuccess = true) override;
	
public:
    virtual void SetObjectiveActive(bool value) override;
};
