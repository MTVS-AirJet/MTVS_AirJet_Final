// Fill out your copyright notice in the Description page of Project Settings.


#include "JBS/J_ObjectiveNeutralizeTarget.h"
#include "Containers/Array.h"
#include "DrawDebugHelpers.h"
#include "Engine/Engine.h"
#include "Engine/EngineTypes.h"
#include "Engine/HitResult.h"
#include "JBS/J_CustomWidgetComponent.h"
#include "JBS/J_GroundTarget.h"
#include "JBS/J_ObjectiveMovePoint.h"
#include "JBS/J_Utility.h"
#include "LHJ/L_Viper.h"
#include "Math/MathFwd.h"
#include <cfloat>
#include "JBS/J_MissionPlayerController.h"
#include "JBS/J_ObjectiveUIComp.h"
#include "Math/UnrealMathUtility.h"
#include "TimerManager.h"


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

void AJ_ObjectiveNeutralizeTarget::BeginPlay()
{
    Super::BeginPlay();
}

void AJ_ObjectiveNeutralizeTarget::InitBindDel()
{
    Super::InitBindDel();

    // 활성화시 서브 이동 목표 생성 바인드
    objectiveActiveDel.AddUObject(this, &AJ_ObjectiveNeutralizeTarget::InitSubMovePoints);
    // 활성화시 지상 목표 생성 바인드
    objectiveActiveDel.AddUObject(this, &AJ_ObjectiveNeutralizeTarget::SpawnGroundTarget);
}

// 과녁 생성
void AJ_ObjectiveNeutralizeTarget::SpawnGroundTarget()
{
    // 스폰 위치 계산
    bool getSpawnTR = CalcSpawnTransform(spawnTR);
    if(!getSpawnTR)
    {
        GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("지상 목표 위치 대체 생성"));
    }

    // 스폰 | 사실상 1개만 스폰
    for(int i = 0; i < spawnTargetAmt; i++)
    {
        auto* groundTarget = GetWorld()->SpawnActor<AJ_GroundTarget>(groundTargetPrefab, spawnTR);

        // 점수 받기 함수 바인드
        groundTarget->sendScoreDel.AddUObject(this, &AJ_ObjectiveNeutralizeTarget::UpdateTargetScore);
        // 활성 함수 바인드
        targetActiveDel.AddDynamic( groundTarget, &AJ_GroundTarget::CRPC_SetTargetActive);
        // XXX 파괴 미사용
        // groundTarget->destroyedDelegate.AddUObject(this, &AJ_ObjectiveNeutralizeTarget::CountTargetDestroyed);
    }

    // 모든 타겟 비활성화
    targetActiveDel.Broadcast(false);
}

bool AJ_ObjectiveNeutralizeTarget::CalcSpawnTransform(FTransform& outSpawnTR)
{
    // 아래로 레이 쏴서 지면 포인트 찾기
    FHitResult outHit;
    ECollisionChannel traceChl = ECC_Visibility;
    FCollisionQueryParams params;
    params.AddIgnoredActor(this);
    // 히트 하는 위치 설정 
    FVector startLocation = this->GetActorLocation();
    FVector endLocation = startLocation + FVector::DownVector * MAX_FLT;
    bool isHit = GetWorld()->LineTraceSingleByChannel(
        outHit,
        startLocation,
        endLocation,
        traceChl,
        params
    );

    if(isHit)
    {
        // 위치
        FVector spLoc = outHit.ImpactPoint;
        // 회전 | 위를 바라보도록
        FQuat spRot = FQuat::FindBetween(FVector::UpVector, outHit.ImpactNormal);
        
        // 스폰 트랜스폼 설정
        outSpawnTR = FTransform(spRot, spLoc, FVector::OneVector);
    }
    // 아래에 충돌이 없으면 그냥 지면 = -4km이라 가정
    else 
    {
        FVector spLoc = this->GetActorLocation();;
        spLoc.Z = 0;

        outSpawnTR = FTransform(FRotator::ZeroRotator, spLoc, FVector::OneVector);
    }

    return isHit;
}


#pragma region 하위 이동 목표 초기화 단
void AJ_ObjectiveNeutralizeTarget::InitSubMovePoints()
{
    if(!HasAuthority() || IS_OBJ_ENDED) return;

    // icon 비활성화
    iconWorldUIComp->SetVisible(false);

    // 현 시점 모든 pc 가져오기
    auto allPC = UJ_Utility::GetAllMissionPC(GetWorld());
    if(allPC.Num() == 0) return;

    // 점수 맵 init
    InitTargetScoreMap(allPC);

    // 편대장 위치 가져오기
    auto* hostPC = allPC[0];
    const auto& baseLocation = hostPC->GetPawn()->GetActorLocation();

    // 해당 위치와 내 위치를 기반으로 사각형으로 도는 트랙으로 하위 이동 목표 스폰

    // 항상 스폰 처리
	FActorSpawnParameters params;
	params.bNoFail = true;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    // 하위 이동 목표 생성
    for(int i = 0; i < subMovePointAmt; i++)
    {
        // 기준 위치 설정 | 처음엔 과녁, 이후는 이전 이동 목표 위치를 기준
        FTransform baseTR;
        if(i == 0)
        {
            // host -> this 방향 구하기
            FVector dir = (this->GetActorLocation() - baseLocation).GetSafeNormal();
            // 해당 방향을 forv로 변경
            baseTR = this->GetActorTransform();
            baseTR.SetRotation(dir.ToOrientationQuat());
        }
        else 
            baseTR = subMPArray[i - 1]->GetActorTransform();

        // 스폰 위치 계산
        auto newSpawnTR = CalcSubMPTransform(baseTR, i);
        
        // 이동 목표 생성
        auto* subMP = SpawnSubMovePoint(newSpawnTR, params);
        if(!subMP) continue;

        // 배열에 추가
        subMPArray.Add(subMP);
    }

    // 서브 목표 시작
    ActiveNextObjective();
    // 시작 보이스 재생
    ReqPlayCommVoice(EMissionProcess::NEUT_TARGET_START, allPC);

    StartNewObjUI();
}

void AJ_ObjectiveNeutralizeTarget::InitTargetScoreMap(const TArray<class AJ_MissionPlayerController*>& allPC)
{
    // 점수 맵 init
    targetScoreMap.Empty();
    for(auto* pc : allPC)
    {
        if(!pc) continue;

        // 과녁 점수 맵 초기화
        targetScoreMap.Add(pc, 0.f);
    }
}

FTransform AJ_ObjectiveNeutralizeTarget::CalcSubMPTransform(const FTransform &baseTR, const int &idx)
{
    FTransform newTR = baseTR;
    // 처음 말고는 왼쪽으로 회전하기
    if(idx != 0)
    {   
        // 왼쪽을 전방벡터로 변환
        FQuat leftRotation = FQuat(FVector::UpVector, FMath::DegreesToRadians(-90.0f));
        newTR.SetRotation(leftRotation * newTR.GetRotation()); // 왼쪽으로 90도 회전
    }

    // idx에 따라 다른 길이 만큼 forv 방향으로 이동
    float addForv = 0;
    // 순서에 따라 다른거 적용
    switch (idx) {
        case 0:
            addForv = firstMPDis;
            break;
        case 2:
            addForv = xMPDis;
            break;
        case 1:
        case 3:
            addForv = yMPDis;
            break;
    }
    // tr의 forv 방향 만큼 이동
    newTR.SetLocation(newTR.GetLocation() + newTR.GetRotation().GetForwardVector() * addForv);

    return newTR;
}

AJ_ObjectiveMovePoint* AJ_ObjectiveNeutralizeTarget::SpawnSubMovePoint(const FTransform &subMPTR, const FActorSpawnParameters &params)
{
    // 이동 목표 생성
    auto* subMP = GetWorld()->SpawnActor<AJ_ObjectiveMovePoint>(movePointPrefab, subMPTR, params);
    if(!subMP) return nullptr;

    // 목표 액터 설정
    subMP->InitObjective(ETacticalOrder::MOVE_THIS_POINT, false);

    // 딜리게이트 바인드
    
    // 완료시 서브 목표 ui 완료 처리
    subMP->objectiveEndDel.AddDynamic(this, &AJ_ObjectiveNeutralizeTarget::EndSubMPUI);
    // 목표 완료시 다음 목표 활성화 바인드
    subMP->objectiveEndDel.AddDynamic(this, &AJ_ObjectiveNeutralizeTarget::ActiveNextObjective);
    // 목표 완료시 보이스 재생 요청
    

    return subMP;
}

#pragma endregion
#pragma endregion

#pragma region 미션 진행 단
/* 
// base 작동
    로컬 폰과의 거리 icon UI 설정 -> (서버단) tick 시작
    수행도 갱신시 갱신된 수행도를 목표 매니저에게 보냄
*/
void AJ_ObjectiveNeutralizeTarget::Tick(float deltaTime)
{
    Super::Tick(deltaTime);
}

#pragma region 하위 이동 목표 진행 단
/*
-> 하위 이동 목표 종료시 현재 진행 인덱스 증가 | ActiveNextObjective
   하위 이동 목표 전부 종료했는지 체크 | CheckEndAllSubMP
   아직 남아있으면 다음 이동 목표 활성화 | ActiveObjectiveByIdx, DelayedObjectiveActive
-> 종료시 과녁 파괴 목표 활성화 | StartHitTarget
*/

void AJ_ObjectiveNeutralizeTarget::ActiveNextObjective()
{
    // GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::White, FString::Printf(TEXT("%d 번째 서브 목표 종료"), CUR_ACTIVE_SUBMP_IDX));

    // 인덱스 증가
	CUR_ACTIVE_SUBMP_IDX++;

    // 하위 이동 목표 전부 완료 체크
    bool isEnd = CheckEndAllSubMP(CUR_ACTIVE_SUBMP_IDX);
    // 서브 이동 목표 전부 끝난거 타격 목표로 전환
    if(isEnd)
        StartHitTarget();
    // 최초가 아니면 종료 애니메이션 대기 (delay 애니메이션에 맞게 수정 필요)
    else
        ActiveObjectiveByIdx(CUR_ACTIVE_SUBMP_IDX, CUR_ACTIVE_SUBMP_IDX == 0);
}

bool AJ_ObjectiveNeutralizeTarget::CheckEndAllSubMP(int idx)
{
    return idx >= subMPArray.Num();
}

void AJ_ObjectiveNeutralizeTarget::ActiveObjectiveByIdx(int mIdx, bool isFirst)
{
	// 목표 액터 가져오기
	auto* obj = subMPArray[mIdx];
	if(!obj) return;

    // @@ 임시 ai 보이스 재생
    auto allPC = UJ_Utility::GetAllMissionPC(GetWorld());

    switch (mIdx) {
        case 0:
            // ReqPlayCommVoice(EMissionProcess::NEUT_TARGET_2_END, allPC);
            break;
        case 1:
            ReqPlayCommVoice(EMissionProcess::NEUT_TARGET_2_END, allPC);
            break;
        case 2:
            ReqPlayCommVoice(EMissionProcess::NEUT_TARGET_3_START, allPC);
            break;
        case 3:
            ReqPlayCommVoice(EMissionProcess::NEUT_TARGET_3_END, allPC);
            break;
    }
	
	// @@ 딜레이 여부 | 애니 끝나는걸 애초에 알면 좋을듯
	float delayTime = isFirst ? 0.01f : 1.5f;

	// 활성화
	DelayedObjectiveActive(obj, delayTime);
}

void AJ_ObjectiveNeutralizeTarget::DelayedObjectiveActive(AJ_BaseMissionObjective *obj, float delayTime)
{
    if(!obj) return;

    // 딜레이 이후 활성화
	FTimerHandle timerHandle2;
	
	GetWorld()->GetTimerManager()
		.SetTimer(timerHandle2, [this, obj]() mutable
	{
		obj->IS_OBJECTIVE_ACTIVE = true;
	}, delayTime, false);
}


void AJ_ObjectiveNeutralizeTarget::StartHitTarget()
{
    // 모든 타겟 활성화
    targetActiveDel.Broadcast(true);
    // icon 활성화
    iconWorldUIComp->SetVisible(true);

    auto allPC = UJ_Utility::GetAllMissionPC(GetWorld());

    // voice 재생
    ReqPlayCommVoice(EMissionProcess::LOCK_ON, allPC);

    for(auto* pc : allPC)
    {
        if(!pc) continue;
        // 과녁 시작 이미지로 변경
        pc->objUIComp->CRPC_DirectSetDetailImg(EMissionProcess::LOCK_ON);

        // @@ 모든 전투기 락온 가능하게 변경
        auto* viper = pc->GetPawn<AL_Viper>();
        if(!viper) continue;
        viper->ClientRPC_LockOnStart();
    }
}

#pragma endregion

#pragma region 과녁 타격 목표 진행 단
/*
-> 과녁에서 GetDamage 받을때 점수 계산 | @@ 베타 구현
-> 계산된 점수 갱신 | UpdateTargetScore
   현재 모든 플레이어가 타격시(== 점수가 0이아님) 인지 체크 | CheckAllTargetHit
   모두 타격 했으면 점수 계산 후 목표 종료 | CalcSuccessPercent
*/
void AJ_ObjectiveNeutralizeTarget::UpdateTargetScore(class AJ_MissionPlayerController *pc, float score)
{
    targetScoreMap[pc] = score;

    bool isEnd = CheckAllTargetHit();
    if(isEnd)
    {
        // 수행도 산정
        SUCCESS_PERCENT = CalcSuccessPercent();
        // 모두 타격했으니 종료
        ObjectiveEnd(true);
    }
}

// 모든 pc 가 과녁 타격했는지 체크
bool AJ_ObjectiveNeutralizeTarget::CheckAllTargetHit()
{
    bool isEnd = true;
    
    // 모든 플레이어 가져오기
    auto allPC = UJ_Utility::GetAllMissionPC(GetWorld());
    for(auto* pc : allPC)
    {
        if(!pc) continue;
        if(!targetScoreMap.Contains(pc))
            targetScoreMap.Add(pc, 0.f);

        // 점수가 0 (== 타격 안함)
        if(targetScoreMap[pc] == 0.f) 
        {
            isEnd = false;
            break;
        }
    }

    return isEnd;
}

float AJ_ObjectiveNeutralizeTarget::CalcSuccessPercent()
{
    // @@ 필요하면 베타때 직접적인 값을 반환할 지도
    // 1. 이동 목표 수행 데이터
    // 수행도 배열
    TArray<float> subMPSPAry;
    Algo::Transform(subMPArray, subMPSPAry, [](AJ_ObjectiveMovePoint* temp){
        if(!temp) return 0.0f;

        return temp->SUCCESS_PERCENT;
    });

    // 2. 과녁 점수 데이터 | 일단 1로 가정 
    TArray<float> targetScoreAry;

    auto allPC = UJ_Utility::GetAllMissionPC(GetWorld());

    Algo::Transform(allPC, targetScoreAry, [this](AJ_MissionPlayerController* temp){
        if(!temp) return 0.0f;

        return targetScoreMap[temp];
    });

    // 이동 목표 수행도의 평균
    float subMPResult = UJ_Utility::CalcAverage(subMPSPAry);
    // 과녁 점수 평균 구하기
    float targetScoreResult = UJ_Utility::CalcAverage(targetScoreAry);

    // 전체 점수의 평균
    return UJ_Utility::CalcAverage({subMPResult, targetScoreResult});
}

#pragma endregion
#pragma endregion

#pragma region 목표 UI 적용 단
/* 특정 타이밍에 목표 UI 시작 | StartNewObjUI
-> 각 pc마다 목표 ui 데이터 설정 | SendObjUIData, SetNeutUIData , pc->crpc 보내기
-> 특정 갱신 타이밍 마다 업데이트 UI | UpdateObjUI -> 이전에 보냈는지 체크후 업데이트 | CheckSendSameData
-> 목표 종료 나 서브 목표 종료시 ui 애니메이션 실행 요청 | EndObjUI , EndSubObjUI, 
-> 목표 종료시 비작동*/

FNeutralizeTargetUIData AJ_ObjectiveNeutralizeTarget::SetNeutUIData(class AJ_MissionPlayerController *pc)
{
    // 서브 이동 목표 수행 여부 데이터
    TArray<FObjSucceedData> subObjData;
    for(auto* subMP : subMPArray)
    {
        subObjData.Add(FObjSucceedData(subMP->IS_OBJ_ENDED, subMP->SUCCESS_PERCENT > 0));
    }

    // 현재 모든 pc 가져오기
    auto allPC = UJ_Utility::GetAllMissionPC(GetWorld());

    int all = allPC.Num();
    int cnt = 0;

    for(auto* onePC : allPC)
    {
        if(!onePC) continue;
        if(!targetScoreMap.Contains(onePC)) continue;

        if(targetScoreMap[onePC] != 0.f) 
            cnt++;
    }

    return FNeutralizeTargetUIData(all, cnt, subObjData);
}

void AJ_ObjectiveNeutralizeTarget::SendObjUIData(class AJ_MissionPlayerController *pc, bool isInit)
{
    Super::SendObjUIData(pc, isInit);

    // 보낼 목표 데이터 구성
    FNeutralizeTargetUIData orderData = SetNeutUIData(pc);
    // ui 생성 시작
    pc->objUIComp->CRPC_StartObjUINeut(orderData);
}

void AJ_ObjectiveNeutralizeTarget::UpdateObjUI()
{
    if(!HasAuthority()) return;

    // 모든 pc 가져오기
    auto allPC = UJ_Utility::GetAllMissionPC(GetWorld());
    // pc에게 새 전술명령 UI 시작 srpc
    for(auto* pc : allPC)
    {
		// 과도한 crpc 방지 처리
		// 이전에 보낸 ui 데이터와 동일한지 검증
        auto orderData = SetNeutUIData(pc);

		// 다르면 업데이트
		// @@ 템플릿 쓸 수 있을지 고민
		FTacticalOrderData temp(this->orderType, orderData);
		bool canUpdate = CheckSendSameData(pc, temp);
        // 데이터 보내기
		if(canUpdate)
			pc->objUIComp->CRPC_UpdateObjUINeut(orderData);
    }
}

// -> 하위 이동 목표 완료시 서브 목표 ui 완료 처리 | EndSubMPUI
void AJ_ObjectiveNeutralizeTarget::EndSubMPUI()
{
    // 해당 서브 목표 종료
    if(CUR_ACTIVE_SUBMP_IDX >= subMPArray.Num()) return;

    auto* subMP = subMPArray[CUR_ACTIVE_SUBMP_IDX];
    if(!subMP) return;

    // 성공 여부 계산
    bool isSuccess = subMP->SUCCESS_PERCENT > 0;

    // ui 업데이트
    UpdateObjUI();

    auto allPC = UJ_Utility::GetAllMissionPC(GetWorld());
    for(auto* pc : allPC)
    {
        if(!pc) continue;

        EndSubObjUI(pc, CUR_ACTIVE_SUBMP_IDX, isSuccess);
        
    }
}

#pragma endregion