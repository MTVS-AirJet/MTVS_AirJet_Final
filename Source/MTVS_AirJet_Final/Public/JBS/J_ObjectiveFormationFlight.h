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
	bool GetIsFormation() {return isFormation;}
	void SetIsFormation(bool value) {isFormation = value;}
		protected:

#pragma region 조건 체크용 값 변수들
	// 조건 충족 값
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Check|Values")
	int checklistValue = 0;

	// 조건 충족 시간
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Check|Values")
	float formationTime = 0.f;
	// 목표 수행중인 시간
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Check|Values")
	float progressTime = 0.f;

	// 조건 체크 갱신 시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Check|Values")
	float checkInterval = 5.f;

	// 조건 1 고도
	// 고도 확인 값
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Check|Height")
	float checkHeight = 400000;
	// 고도 오차 허용 값
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Check|Height")
	float checkHeightAdj = 60000;

#pragma endregion
public:

protected:
#pragma region 시작 설정 단
/* icon UI 캐시 -> (서버단) 딜리게이트 바인드 -> init 목표 초기 설정 -> set active(false)
*/
    virtual void BeginPlay() override;

	virtual void ObjectiveActive() override;

	// 생성할 때 딜리게이트 바인드 공간
	virtual void InitBindDel() override;
#pragma endregion

#pragma region 목표 진행 단
/* 목표 활성화 중 편대 조건 체크 1,2 | CheckFormation
-> 조건 타입에 따라 다른 조건 함수 실행 | CheckBase
-> 고도 체크 | CheckHeight
-> 진형 체크 | CheckAlign -> 플레이어 역할에 따라 다른 조건 함수 실행 | CheckForward, CheckLeftRight
*/
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

	// 성공 판정 충돌 체크
	virtual void OnCheckCapsuleBeginOverlap(
		UPrimitiveComponent *OverlappedComponent, AActor *OtherActor,
		UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep,
		const FHitResult &SweepResult) override;
#pragma endregion

#pragma region 목표 UI 적용 단
/* 특정 타이밍에 목표 UI 시작 | StartNewObjUI
-> 각 pc마다 목표 ui 데이터 설정 | SendObjUIData, SetFormationUIData, CRPC_StartObjUIFormation
-> 특정 갱신 타이밍 마다 업데이트 UI | UpdateObjUI -> 이전에 보냈는지 체크후 업데이트 | CheckSendSameData
-> 목표 종료 나 서브 목표 종료시 ui 애니메이션 실행 요청 | EndObjUI , EndSubObjUI
-> 목표 종료시 비작동*/

	// 한 pc에게 목표 UI 데이터 보내기
    virtual void SendObjUIData(class AJ_MissionPlayerController *pc, bool isInit = false) override;

	// ui data 구성
	UFUNCTION(BlueprintCallable)
	FFormationFlightUIData SetFormationUIData(class AJ_MissionPlayerController *pc = nullptr);

    // 목표 UI 값 갱신 | 수행도 갱신 시 호출
    virtual void UpdateObjUI() override;
#pragma endregion

    public:
        virtual void Tick(float deltaTime) override;
};
