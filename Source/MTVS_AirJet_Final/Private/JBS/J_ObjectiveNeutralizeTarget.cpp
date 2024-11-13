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
#include "Math/MathFwd.h"
#include <cfloat>
#include "JBS/J_MissionPlayerController.h"
#include "JBS/J_ObjectiveUIComp.h"
#include "Math/UnrealMathUtility.h"
#include "TimerManager.h"

void AJ_ObjectiveNeutralizeTarget::BeginPlay()
{
    Super::BeginPlay();

    // 활성화시 서브 이동 목표 생성 바인드
    objectiveActiveDel.AddUObject(this, &AJ_ObjectiveNeutralizeTarget::InitSubMovePoints);
    // 활성화시 지상 목표 생성 바인드
    objectiveActiveDel.AddUObject(this, &AJ_ObjectiveNeutralizeTarget::SpawnGroundTarget);
    // 목표 완료시 목표 UI 완료 바인드
    objectiveEndDel.AddUObject(this, &AJ_ObjectiveNeutralizeTarget::SRPC_EndObjUI);
}

void AJ_ObjectiveNeutralizeTarget::InitSubMovePoints()
{
    // 현 시점 모든 pc 저장
    allPC = UJ_Utility::GetAllMissionPC(GetWorld());
    // 점수 맵 init
    for(auto* pc : allPC)
    {
        targetScoreMap.Add(pc, 0.f);
    }


    // 편대장 위치 가져오기 | 호스트 가져옴
    AJ_MissionPlayerController* hostPC;
    UJ_Utility::GetLocalPlayerController(GetWorld(), hostPC);
    const auto& hostLocation = hostPC->GetPawn()->GetActorLocation();

    
    // 하위 목표 생성 및 배치
    // 항상 스폰 처리
	FActorSpawnParameters params;
	params.bNoFail = true;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    for(int i = 0; i < subMovePointAmt; i++)
    {
        // 기준 위치 설정 | 처음엔 과녁, 이후는 이전 이동 목표 위치 기준
        FTransform baseTR;
        if(i == 0)
        {
            // host -> this 방향 구하기
            FVector dir = (this->GetActorLocation() - hostLocation).GetSafeNormal();
            // 해당 방향을 forv로 변경
            baseTR = this->GetActorTransform();
            baseTR.SetRotation(dir.ToOrientationQuat());
        }
        else {
            baseTR = subMPArray[i - 1]->GetActorTransform();
        }

        // 스폰 위치 계산
        auto newSpawnTR = CalcSubMPTransform(baseTR, i);
        // 이동 목표 생성
        auto* subMP = GetWorld()->SpawnActor<AJ_ObjectiveMovePoint>(movePointPrefab, newSpawnTR, params);
        // 목표 액터 설정
        subMP->InitObjective(ETacticalOrder::MOVE_THIS_POINT, false);

        // 딜리게이트 바인드
        // 목표 완료시 다음 목표 활성화 바인드
		subMP->objectiveEndDel.AddUObject(this, &AJ_ObjectiveNeutralizeTarget::ActiveNextObjective);
		// // 목표 수행도 갱신함수 바인드
		// objectiveActor->sendObjSuccessDel.AddUObject(this, &UJ_ObjectiveManagerComponent::UpdateObjectiveSuccess);

        // 배열에 추가
        subMPArray.Add(subMP);
    }

    // 자기 위치 아이콘 비활성화
    // this->iconWorldUIComp->bHiddenInGame = true;
    // MRPC_SetVisibleIconUI(false);


    // 서브 목표 시작
    ActiveNextObjective();

    SRPC_StartNewObjUI();
}

void AJ_ObjectiveNeutralizeTarget::ActiveNextObjective()
{
    // GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::White, FString::Printf(TEXT("%d 번째 서브 목표 종료"), CUR_ACTIVE_SUBMP_IDX));

    // 최초 이후 서브 목표 종료
    if(CUR_ACTIVE_SUBMP_IDX > -1 && CUR_ACTIVE_SUBMP_IDX < subMPArray.Num())
    {
        // 성공 여부
        bool isSuccess = subMPArray[CUR_ACTIVE_SUBMP_IDX]->SUCCESS_PERCENT > 0;
        for(auto* pc : allPC)
        {
            SRPC_EndSubObjUI(pc, CUR_ACTIVE_SUBMP_IDX, isSuccess);
        }
    }

    // 인덱스 증가
	CUR_ACTIVE_SUBMP_IDX++;

	// 최초가 아니면 종료 애니메이션 대기 (delay 애니메이션에 맞게 수정 필요)
	ActiveObjectiveByIdx(CUR_ACTIVE_SUBMP_IDX, CUR_ACTIVE_SUBMP_IDX == 0);

}

void AJ_ObjectiveNeutralizeTarget::ActiveObjectiveByIdx(volatile int mIdx, bool isFirst)
{
    if(mIdx >= subMPArray.Num())
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("objAry out of range"));

        // 서브 이동 목표 전부 끝난거 타격 목표로 전환
        iconWorldUIComp->SetVisible(true);
        // FIXME 
        isSubEnd = true;
        debugCheck = true;
		return;
	}
    
	// 목표 액터 가져오기
	auto* obj = subMPArray[mIdx];
	if(!obj) return;
	
	// @@ 딜레이 여부 | 애니 끝나는걸 애초에 알면 좋을듯
	float delayTime = isFirst ? 0.01f : 1.5f;

	// 활성화
	DelayedObjectiveActive(obj, delayTime);
}

void AJ_ObjectiveNeutralizeTarget::DelayedObjectiveActive(AJ_BaseMissionObjective *obj, float delayTime)
{
    check(obj);
	FTimerHandle timerHandle2;
	
	GetWorld()->GetTimerManager()
		.SetTimer(timerHandle2, [this, obj]() mutable
	{
		obj->IS_OBJECTIVE_ACTIVE = true;
		// GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString::Printf(TEXT("asdasd %s"), *obj->GetName()));
	}, delayTime, false);
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

        // GEngine->AddOnScreenDebugMessage(-1, 333.f, FColor::Red, FString::Printf(TEXT("새 tr : %s"), *newTR.ToString()));
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
        // 회전
        // FRotator spRot = outHit.ImpactNormal.Rotation();
        FQuat spRot = FQuat::FindBetween(FVector::UpVector, outHit.ImpactNormal);
        
        // 스폰 트랜스폼 설정
        outSpawnTR = FTransform(spRot, spLoc, FVector::OneVector);
    }
    // @@ 그냥 지면 = -4km이라 가정
    else {
        FVector spLoc = this->GetActorLocation() + FVector::DownVector * UJ_Utility::defaultMissionObjectHeight;
        outSpawnTR = FTransform(FRotator::ZeroRotator, spLoc, FVector::OneVector);
    }

    return isHit;
}

// 활성화 딜리게이트에서 실행됨
void AJ_ObjectiveNeutralizeTarget::SpawnGroundTarget()
{
    


    // 스폰 위치 계산
    bool getSpawnTR = CalcSpawnTransform(spawnTR);
    if(!getSpawnTR)
    {
        GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("지상 목표 위치 대체 생성"));
    }

    // solved gt 캐시 해야 하려나?
    // XXX 여러개 소환하려면 분산시켜야 할 듯
    for(int i = 0; i < spawnTargetAmt; i++)
    {
        auto* groundTarget = GetWorld()->SpawnActor<AJ_GroundTarget>(groundTargetPrefab, spawnTR);
        // GEngine->AddOnScreenDebugMessage(-1, 333.f, FColor::Green, FString::Printf(TEXT("스폰 위치 %s"), *spawnTR.ToString()));
        // 파괴 딜리게이트에 함수 넣기
        groundTarget->destroyedDelegate.AddUObject(this, &AJ_ObjectiveNeutralizeTarget::CountTargetDestroyed);
        // 점수 받기 함수 넣기
        groundTarget->sendScoreDel.AddUObject(this, &AJ_ObjectiveNeutralizeTarget::UpdateTargetScore);
    }
}

// XXX 이제 사용 안함
void AJ_ObjectiveNeutralizeTarget::CountTargetDestroyed()
{
    destroyedTargetAmt++;

    // 수행도 갱신
    SUCCESS_PERCENT = static_cast<float>(destroyedTargetAmt) / spawnTargetAmt;

    // GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::White, FString::Printf(TEXT("현재 파괴된 수 : %d, 남은 수 : %d"), destroyedTargetAmt, (spawnTargetAmt - destroyedTargetAmt)));

    if(destroyedTargetAmt == spawnTargetAmt)
    {
        this->ObjectiveEnd(true);
    }
}

void AJ_ObjectiveNeutralizeTarget::Tick(float deltaTime)
{
    Super::Tick(deltaTime);

    // FIXME 이거 대체 왜 클라에서 안 꺼지는 거임
    if(debugCheck)
    {
        MRPC_SetVisibleIconUI(isSubEnd);
        debugCheck = false;
    }
}


void AJ_ObjectiveNeutralizeTarget::SRPC_StartNewObjUI()
{
    Super::SRPC_StartNewObjUI();

    // 보낼 데이터 
    auto orderData = SetObjUIData();

    // pc에게 새 전술명령 UI 시작 srpc
    for(auto* pc : allPC)
    {
        pc->objUIComp->CRPC_StartObjUI(orderData);
    }
}

void AJ_ObjectiveNeutralizeTarget::SRPC_UpdateObjUI()
{
    Super::SRPC_UpdateObjUI();
}

void AJ_ObjectiveNeutralizeTarget::SRPC_EndObjUI()
{
    Super::SRPC_EndObjUI();
}

// void AJ_ObjectiveNeutralizeTarget::SRPC_EndSubObjUI()
// {
//     Super::SRPC_EndSubObjUI();
// }

FTacticalOrderData AJ_ObjectiveNeutralizeTarget::SetObjUIData(class AJ_MissionPlayerController *pc)
{
    // 서브 이동 목표 수행 여부 데이터
    TArray<FObjSucceedData> subObjData;
    for(auto* subMP : subMPArray)
    {
        subObjData.Add(FObjSucceedData(subMP->IS_OBJ_ENDED, subMP->SUCCESS_PERCENT > 0));
    }

    // 모든 pc 가 타격시 목표 종료
    int all = allPC.Num();
    int cnt = 0;
    for(auto* onePC : allPC)
    {
        if(targetScoreMap[onePC] != 0.f) 
            cnt++;
    }

    FNeutralizeTargetUIData data(all, cnt, subObjData);

    return FTacticalOrderData(this->orderType, FFormationFlightUIData(), data);
}

void AJ_ObjectiveNeutralizeTarget::UpdateTargetScore(class AJ_MissionPlayerController *pc, float score)
{
    targetScoreMap[pc] = score;

    // 모든 pc 가 타격시 목표 종료
    for(auto* onePC : allPC)
    {
        if(targetScoreMap[onePC] == 0.f) 
            return;
    }
    // 수행도 산정
    SUCCESS_PERCENT = CalcSuccessPercent();
    // 모두 타격했으니 종료
    ObjectiveEnd(true);
}

float AJ_ObjectiveNeutralizeTarget::CalcSuccessPercent()
{
    // @@ 필요하면 베타때 직접적인 값을 반환할 지도
    // 1. 이동 목표 수행 데이터
    // 수행도 배열
    TArray<float> subMPSPAry;
    Algo::Transform(subMPArray, subMPSPAry, [](AJ_ObjectiveMovePoint* temp){
        return temp->SUCCESS_PERCENT;
    });
    //캐스트 후
    // 수행도의 평균
    float subMPResult = UJ_Utility::CalcAverage(subMPSPAry);

    // 2. 과녁 점수 데이터 | 일단 1로 가정 
    TArray<float> targetScoreAry;
    Algo::Transform(allPC, targetScoreAry, [this](AJ_MissionPlayerController* temp){
        return targetScoreMap[temp];
    });
    //캐스트 후
    float targetScoreResult = UJ_Utility::CalcAverage(targetScoreAry);

    // 과녁 점수의 평균
    return UJ_Utility::CalcAverage({subMPResult, targetScoreResult});
}

void AJ_ObjectiveNeutralizeTarget::SetObjectiveActive(bool value)
{
    Super::SetObjectiveActive(value);

    iconWorldUIComp->SetVisible(false);
}
