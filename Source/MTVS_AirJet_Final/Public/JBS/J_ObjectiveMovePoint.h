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

	// 빔 이펙트 메시
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Component")
	class UStaticMeshComponent* beamMeshComp;

	// 빔 길이 | 체크 컴포넌트의 값에서 가져옴
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
	float beamLength;

	// 빔 충돌 판정 폭 | 체크 컴포넌트의 값에서 가져옴
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
	float beamRadius;

	// 실패 판정할 기준 방향 | 목표 <- 편대장 의 방향
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Values")
	FVector baseDirection;

	// 실패 판정할 거리 | 일단 100m
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
	float failDis = 10000.f;

public:

protected:
#pragma region 시작 설정 단
/* MOVEPOINT는 하위 목표나 상속받아 편대비행으로 사용하므로 UI DATA 전송은 하지 않음
   따라서 딜리게이트 바인드 설정 | InitBindDel (편대비행은 따로 처리)
-> 성공 체크할 캡슐 overlap 함수 바인드
-> 목표 활성 유무에 따라 체크 캡슐 및 빔 이펙트 메시 활성화 동기화 | SetObjectiveActive, MRPC_SetVisibleBeam
-> 목표 활성화시 실패 판정 방향 설정(편대장 기준) | ObjectiveActive, SetBaseDir
*/
	virtual void BeginPlay();

	// 생성할 때 딜리게이트 바인드 공간
    virtual void InitBindDel() override;

	virtual void ObjectiveActive();

	// 빔 메시 활/비활성화
	UFUNCTION(NetMulticast, Reliable)
	virtual void MRPC_SetVisibleBeam(bool value);

        // 기준 방향 설정 | 호스트 전투기 기준
	FVector SetBaseDir();
#pragma endregion

#pragma region tick 목표 수행 단
/* tick 활성화시 실패 판정 체크 | CheckFail, ObjectiveEnd
-> 체크 캡슐에 충돌시 성공 처리 | OnCheckCapsuleBeginOverlap, ObjectiveEnd
*/
	// 실패 체크 | 기준 방향 보다 현재 상대 위치가 넘어가면 실패
    bool CheckFail(const FVector &baseDir);

	// 성공 판정 충돌
    UFUNCTION()
    virtual void OnCheckCapsuleBeginOverlap(
		UPrimitiveComponent *OverlappedComponent, AActor *OtherActor,
		UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep,
		const FHitResult &SweepResult);

public:
    virtual void Tick(float deltaTime) override;
    virtual void SetObjectiveActive(bool value) override;

#pragma endregion
};
