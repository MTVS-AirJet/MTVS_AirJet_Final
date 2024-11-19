// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "JBS/J_BaseMissionObjective.h"
#include "J_MissionPlayerController.h"
#include "J_ObjectiveNeutralizeTarget.generated.h"

UCLASS()
class MTVS_AIRJET_FINAL_API AJ_ObjectiveNeutralizeTarget : public AJ_BaseMissionObjective
{
	GENERATED_BODY()
/*
	하위 이동목표(사격형 모양) 4개 + 타격 목표 로 구성되어있음
	타격 목표는 내장되어 있고, 이동목표는 movepoint를 배열로 관리
*/
protected:
	// 스폰 지점
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Values")
	FTransform spawnTR;

#pragma region 서브 이동 목표 관련 변수
	// 이동 목표 프리팹
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|SubMP|Classes")
	TSubclassOf<class AJ_ObjectiveMovePoint> movePointPrefab; 

	// 서브 이동 목표 개수 == 4
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|SubMP|Values")
	int subMovePointAmt = 4;

	// 현재 진행 중인 서브 목표 인덱스
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|SubMP|Values")
	int curActiveSubMPIdx = -1;
		public:
	__declspec(property(get = GetCurSubMPIdx, put = SetCurSubMPIdx)) int CUR_ACTIVE_SUBMP_IDX;
	int GetCurSubMPIdx() {return curActiveSubMPIdx;}
	void SetCurSubMPIdx(int value) {curActiveSubMPIdx = value;}
		protected:

	// 과녁 -> 0번 기둥 거리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|SubMP|Values")
	float firstMPDis = 6825;
	// 0->1 , 2->3 세로 기둥 거리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|SubMP|Values")
	float yMPDis = 9150;
	// 1->2 가로 기둥 거리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|SubMP|Values")
	float xMPDis = 27300;

	// 하위 이동 목표 배열
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|SubMP|Objects")
	TArray<class AJ_ObjectiveMovePoint*> subMPArray;

#pragma endregion

#pragma region 과녁 관련 변수
	// 지상 타겟 프리팹
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Target|Classes")
	TSubclassOf<class AJ_GroundTarget> groundTargetPrefab;

	// XXX 생성 개수 | 과녁으로 변경해서 사실상 고정
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Target|Values")
	int spawnTargetAmt = 1;

	// 과녁 점수 맵
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Target|Values")
	TMap<class AJ_MissionPlayerController*, float> targetScoreMap;
#pragma endregion
public:

protected:
#pragma region 시작 설정 단
/* 
// base 시작 단
icon UI 캐시 | SetTargetIconUI
-> (서버단) 딜리게이트 바인드 | InitBindDel 
-> init 목표 초기 설정 | InitObjective
-> set active(false) 


-> 목표 활성화시 과녁 생성 | SpawnGroundTarget
   스폰위치 계산 | CalcSpawnTransform

-> 목표 활성화시 서브 목표 생성 | InitSubMovePoints
-> 모든 pc 가져와서 점수 맵 초기화 | InitTargetScoreMap
-> 편대장 기준 서브 이동 목표 생성
   생성 위치 계산 | CalcSubMPTransform
   하위 이동 목표 생성(4회 반복) | SpawnSubMovePoint * 4 ==> 이후 첫 이동 목표 활성화 ActiveNextObjective
*/

    virtual void BeginPlay() override;

	// 생성할 때 딜리게이트 바인드 공간
    virtual void InitBindDel() override;

	// 지상 타겟 스폰
	UFUNCTION(BlueprintCallable)
    void SpawnGroundTarget();

    // 스폰 지점 계산
    bool CalcSpawnTransform(FTransform& outSpawnTR);



	// 점수 맵 초기화 및 서브 하위 목표들 생성 | 아래 함수 반복
	UFUNCTION(BlueprintCallable)
	void InitSubMovePoints();

	// 점수 맵 초기화
	void InitTargetScoreMap(const TArray<class AJ_MissionPlayerController*>& allPC);

	// 서브 이동 목표 위치 구하기 | 기준 위치, 인덱스
	FTransform CalcSubMPTransform(const FTransform &baseTR, const int &idx);

	// 서브 이동 목표 하나 생성 및 초기 값 설정
	class AJ_ObjectiveMovePoint* SpawnSubMovePoint(const FTransform &subMPTR, const FActorSpawnParameters &params);


    

	

	
#pragma endregion

#pragma region 미션 진행 단
/* 
// base 작동
    로컬 폰과의 거리 icon UI 설정 -> (서버단) tick 시작
    수행도 갱신시 갱신된 수행도를 목표 매니저에게 보냄
*/
/*
-> 하위 이동 목표 종료시 현재 진행 인덱스 증가 | ActiveNextObjective
   하위 이동 목표 전부 종료했는지 체크 | CheckEndAllSubMP
   아직 남아있으면 다음 이동 목표 활성화 | ActiveObjectiveByIdx, DelayedObjectiveActive
-> 종료시 과녁 파괴 목표 활성화 | StartHitTarget
*/

	// 다음 서브 목표 활성화
	UFUNCTION(BlueprintCallable)
	void ActiveNextObjective();

	// 하위 이동 목표 전체 수행 유무 체크
	bool CheckEndAllSubMP(int idx);

	// 해당 인덱스 서브 목표 활성화
	void ActiveObjectiveByIdx(int mIdx, bool isFirst = false);

	// 지연 시간 후 서브 목표 활성화
	void DelayedObjectiveActive(AJ_BaseMissionObjective *obj, float delayTime);

	// 과녁 타격 활성화
	void StartHitTarget();
/*
-> 과녁에서 GetDamage 받을때 점수 계산 | @@ 베타 구현
-> 계산된 점수 갱신 | UpdateTargetScore
   현재 모든 플레이어가 타격시(== 점수가 0이아님) 인지 체크 | CheckAllTargetHit
   모두 타격 했으면 점수 계산 후 목표 종료 | CalcSuccessPercent
*/

	// 과녁 점수 갱신
	UFUNCTION(BlueprintCallable)
	void UpdateTargetScore(class AJ_MissionPlayerController *pc, float score);

	// 모든 플레이어가 과녁 타격했는지 체크
	bool CheckAllTargetHit();

	// 수행도 계산
	float CalcSuccessPercent();
#pragma endregion

#pragma region 목표 UI 적용 단
/* 특정 타이밍에 목표 UI 시작 | StartNewObjUI
-> 각 pc마다 목표 ui 데이터 설정 | SendObjUIData, SetNeutUIData , pc->crpc 보내기
-> 특정 갱신 타이밍 마다 업데이트 UI | UpdateObjUI -> 이전에 보냈는지 체크후 업데이트 | CheckSendSameData
-> 목표 종료 나 서브 목표 종료시 ui 애니메이션 실행 요청 | EndObjUI , EndSubObjUI, 
-> 목표 종료시 비작동*/

	// 목표 UI에 표시할 데이터 구성
	virtual FNeutralizeTargetUIData SetNeutUIData(class AJ_MissionPlayerController *pc = nullptr);

	// 한 pc에게 목표 UI 데이터 보내기 | 상속하면 재정의
	virtual void SendObjUIData(class AJ_MissionPlayerController *pc, bool isInit = false) override;

	// 목표 UI 값 갱신 | 수행도 갱신 시 호출
	virtual void UpdateObjUI() override;

	// 서브 이동 목표 완료시 sub ui 완료 처리
	UFUNCTION(BlueprintCallable)
	void EndSubMPUI();
#pragma endregion

public:
	virtual void Tick(float deltaTime) override;
};
