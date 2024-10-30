// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "JBS/J_ObjectiveMovePoint.h"
#include "J_ObjectiveFormationFlight.generated.h"

/**
 * 
 */
UCLASS()
class MTVS_AIRJET_FINAL_API AJ_ObjectiveFormationFlight : public AJ_ObjectiveMovePoint
{
	GENERATED_BODY()
protected:
	// 조건 1 고도 : 4km , +- 60 미터 오차 가능
	// 조건 2 진형 유지 : 역할에 따라 위치하도록
	// 리더 == 내 앞에 다른 팀원 없음
	// lt 윙어 == 내 왼쪽에 아무도 없음
	// rt 윙어 == 내 오른쪽에 아무도 없음

	// 편대 비행 조건 만족 여부
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Values")
	bool isFormation = true;
		public:
	__declspec(property(get = GetIsFormation, put = SetIsFormation)) bool IS_FORMATION;
	bool GetIsFormation()
	{
		return isFormation;
	}
	void SetIsFormation(bool value)
	{
		isFormation = value;
	}
		protected:
	// 조건 충족 값
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Values")
	int checklistValue = 0;

	// 조건 충족 시간
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Values")
	float formationTime = 0.f;
	// 목표 수행중인 시간
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Values")
	float progressTime = 0.f;

	// 플레이어 목록
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Objects")
	TArray<class APawn*> allPawns;

	// 조건 1 고도
	// 고도 확인 값
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Check|Height")
	float checkHeight = 400000;
	// 고도 오차 값
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Check|Height")
	float checkHeightAdj = 60000;
public:

protected:
    virtual void BeginPlay() override;
    // 편대 조건 체크
    bool CheckFormation();

	// 베이스 조건
    int CheckBase(EFormationChecklist checkType);
    // 조건 1 고도 맞추기
    bool CheckHeight();
	// 조건 2 진형 맞추기
    bool CheckAlign();

	// 전방에 있는지 확인
    bool CheckForward(AActor* actor, AActor* otherActors);
	// 좌우에 있는지 확인
    bool CheckLeftRight(AActor *actor, AActor* otherActors, bool checkLeft = true);

	// 목표 UI 시작 | 목표 활성화 시 호출
    virtual void SRPC_StartNewObjUI() override;
    // 목표 UI 값 갱신 | 수행도 갱신 시 호출
    virtual void SRPC_UpdateObjUI() override;
    // 목표 UI 완료 | 목표 완료시 호출
    virtual void SRPC_EndObjUI() override;
    // 목표 서브 조건 UI 완료
    virtual void SRPC_EndSubObjUI() override;

	// 성공 판정 충돌
	virtual void OnCheckCapsuleBeginOverlap(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor,
											UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep,
											const FHitResult &SweepResult) override;

    public:
        virtual void Tick(float deltaTime) override;
};
