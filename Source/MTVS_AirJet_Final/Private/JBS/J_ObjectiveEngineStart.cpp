// Fill out your copyright notice in the Description page of Project Settings.


#include "JBS/J_ObjectiveEngineStart.h"
#include "Engine/Engine.h"
#include "GraphEditAction.h"
#include "JBS/J_MissionPlayerController.h"
#include "JBS/J_Utility.h"

void AJ_ObjectiveEngineStart::BeginPlay()
{
    Super::BeginPlay();

}

void AJ_ObjectiveEngineStart::ObjectiveActive()
{
    if(!HasAuthority()) return;

    GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("1. 시동 절차 시작"));
    // 현재 존재하는 모든 pc 가져오기
    allPC = UJ_Utility::GetAllMissionPC(GetWorld());
 
    // 수행 데이터 맵 초기화
    allData.InitDataMap(allPC);

    // 모든 pc 에게 엔진 수행 딜리게이트 바인드
    for(auto* pc : allPC)
    {
        pc->sendEngineProgDel.BindUObject(this, &AJ_ObjectiveEngineStart::CheckProgress);
    }

    // 목표 종료시 수행도 결과 계산 바인드
    objectiveEndDel.AddUObject(this, &AJ_ObjectiveEngineStart::CalcSuccessPercent);

    Super::ObjectiveActive();
}

void AJ_ObjectiveEngineStart::Tick(float deltaTime)
{
    Super::Tick(deltaTime);

}

void AJ_ObjectiveEngineStart::CheckProgress(class AJ_MissionPlayerController *pc, EEngineProgress type)
{
    // 해당 pc 가 수행중인 type 과 일치하면 성공 처리 후 다음 수행으로 넘어가기
    bool isValid = allData.dataMap.Contains(pc);
    check(isValid);
    auto& data = allData.dataMap[pc];
    // 성공 여부
    bool isSuccess = data.curProgress == type;
    int enumIdx = static_cast<int>(data.curProgress) - 1;

    // 해당 인덱스 서브 목표 완료 처리
    SRPC_EndSubObjUI(pc, enumIdx, isSuccess);

    GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::White, FString::Printf(TEXT("다음 절차 수행, 성공 여부 : %s"), *UJ_Utility::ToStringBool(isSuccess)));

    // 성공처리
    ActiveNextProgress(data, isSuccess);

    // 수행도 점수 처리
    CalcSuccessPercent();

    // 모두가 스탠 바이 상태 이상 다음 으로 진행 처리
    if(!isReadyTakeOff && CheckAllRunEngine(allPC, EEngineProgress::STANDBY_OTHER_PLAYER))
    {
        isReadyTakeOff = true;
        GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("전부 대기중"));
        
        for(auto* onePC : allPC)
        {
            auto& oneData = allData.dataMap[onePC];
            CheckProgress(onePC, oneData.curProgress);
        }
    }

    // 모든 pc가 이륙 대기 상태가 되면 종료 처리
    if(CheckAllRunEngine(allPC, EEngineProgress::TAKE_OFF))
    {
        GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("이륙 준비 완료"));

        ObjectiveEnd(true);
        return;
    }
}

bool AJ_ObjectiveEngineStart::CheckAllRunEngine(const TArray<class AJ_MissionPlayerController *> pcs, EEngineProgress checkType)
{
    // 해당 절차 이상인지 전부 체크
    bool isAllEnd = true;
    for(auto* onePC : pcs)
    {
        if(allData.dataMap[onePC].curProgress < checkType)
        {
            isAllEnd = false;
            break;
        }
    }

    return isAllEnd;
}

void AJ_ObjectiveEngineStart::ActiveNextProgress(FEngineProgressData& data, bool isSuccess)
{
    // 성공시 수행도 추가
    if(isSuccess)
        data.AddSuccessValue(data.curProgress);
    
    // 다음 수행도로 변경
    data.SetNextProgress();
}

void AJ_ObjectiveEngineStart::CalcSuccessPercent()
{
    // 모든 pc 데이터를 계산
    float totalRate = 0.f;
    for(auto* pc : allPC)
    {
        auto& data = allData.dataMap[pc];
        // 성공 비율 계산
        float rate = data.CalcSuccessRate(data.successValue);

        totalRate += rate;
    }

    // 평균 계산
    float avg = totalRate / allPC.Num();
    // 수행도에 적용
    this->SUCCESS_PERCENT = avg;

    // GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::White, FString::Printf(TEXT("수행도 결과 : %.2f"), avg));
}

FTacticalOrderData AJ_ObjectiveEngineStart::SetObjUIData(AJ_MissionPlayerController *pc)
{
    // 각자 현재 절차 값
    FEngineProgressData& data = allData.dataMap[pc];

    return FTacticalOrderData(this->orderType, data);
}