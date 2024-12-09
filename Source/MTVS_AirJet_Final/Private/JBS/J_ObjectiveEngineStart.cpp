// Fill out your copyright notice in the Description page of Project Settings.


#include "JBS/J_ObjectiveEngineStart.h"
#include "Engine/Engine.h"
#include "GraphEditAction.h"
#include "JBS/J_MissionPlayerController.h"
#include "JBS/J_MissionGamemode.h"
#include "JBS/J_Utility.h"
#include "JBS/J_ObjectiveUIComp.h"

#pragma region 시작 단
/* begin -> active 엔진 수행 데이터 초기화, 수행 딜리게이트 바인드
-> 목표 종료시 수행도 계산 -> */
void AJ_ObjectiveEngineStart::BeginPlay()
{
    Super::BeginPlay();

    if(!this->HasAuthority()) return;

}

void AJ_ObjectiveEngineStart::ObjectiveActive()
{
    Super::ObjectiveActive();

    // 현재 존재하는 모든 pc 가져오기
    auto allPC = UJ_Utility::GetAllMissionPC(GetWorld());
 
    // 수행 데이터 맵 초기화
    allEngineProgData.InitDataMap(allPC);

    // 모든 pc 에게 엔진 수행 딜리게이트 바인드
    for(auto* pc : allPC)
    {
        if(!pc) continue;

        pc->sendEngineProgDel.BindUObject(this, &AJ_ObjectiveEngineStart::CheckProgress);
    }

    // 목표 종료시 수행도 결과 계산 바인드
    objectiveEndDel.AddDynamic(this, &AJ_ObjectiveEngineStart::CalcSuccessPercent);

    // 목표 ui 신규 갱신
    StartNewObjUI();    
    // 최초 mic 보이스 안내 | 이후는 ActiveNextProgress 에서
    ReqPlayCommVoice(EMissionProcess::MIC_SWITCH_ON, allPC);
}
#pragma endregion


#pragma region 반복 단
/* viper 쪽에서 시동 절차 수행하면 딜리게이트 실행 | CheckProgress
-> 다음 수행으로 넘어가기 및 점수 추가 | ActiveNextProgress, CalcSuccessPercent
-> 모두 대기 절차 면 넘어가기 & 이륙 절차면 목표 종료 | CheckAllRunEngine*/
void AJ_ObjectiveEngineStart::Tick(float deltaTime)
{
    Super::Tick(deltaTime);
}

void AJ_ObjectiveEngineStart::CheckProgress(class AJ_MissionPlayerController *pc, EEngineProgress type)
{
    // 수행 맵에 존재 확인
    if(IS_OBJ_ENDED 
        || !pc
        || !allEngineProgData.dataMap.Contains(pc)) return;

    // 해당 pc의 현재 진핸 단계와 type이 일치하면 성공 / 아님 실패
    auto& data = allEngineProgData.dataMap[pc];
    // 성공 여부
    bool isSuccess = data.curProgress == type;
    int enumIdx = static_cast<int>(data.curProgress) - 1;
    // 해당 인덱스 서브 목표 완료 처리
    EndSubObjUI(pc, enumIdx, isSuccess);

    // FString debugStr = FString::Printf(TEXT("pc : %s, 현재 절차 : %s, 성공 여부 : %s")
    //     , *pc->GetName()
    //     , *UJ_Utility::ToStringEnumPure(type)
    //     , *UJ_Utility::ToStringBool(isSuccess));
    // UJ_Utility::PrintFullLog(debugStr, 3.f, FColor::White);

    
    // 다음 수행으로 넘어가기
    ActiveNextProgress(data, isSuccess);

    // 스탠바이 상태가 되면 팝업 활성화
    if(data.curProgress == EEngineProgress::RELEASE_SIDE_BREAK)
    {
        pc->objUIComp->CRPC_ActivePopupUI(EMissionProcess::STANDBY_OTHER_PLAYER);
    }

    // 다음 지휘관 보이스 실행
    // 시동 절차 -> 미션 인덱스로 변환
    int mpIdx = UJ_Utility::ConvertEngineProgressToMissionProcessIdx(data.curProgress);
    ReqPlayCommVoice(mpIdx, {pc});

    // 사이드 브레이크 수행이 들어왔을때 현재 진행이 그거 이전일때 takeoff로 설정 | 대기 무시하고 다 켜버리는 경우
    if(!isSuccess && type == EEngineProgress::RELEASE_SIDE_BREAK && data.curProgress <= type)
    {
        data.curProgress = EEngineProgress::TAKE_OFF;
    }
    // @@ 위 코드로 해결되는지 확인 필요
    // // 대기 상태일때 그냥 사이드 켜버리면 넘어가게 처리 
    // if(!isSuccess && data.curProgress == EEngineProgress::RELEASE_SIDE_BREAK)
    // {
    //     CheckProgress(pc, data.curProgress);
    // }

    // 전체 수행도 점수 갱신
    CalcSuccessPercent();

    // 전체 플레이어 체크 ( 대기, 종료 )
    auto allPC = UJ_Utility::GetAllMissionPC(GetWorld());


    // 모두가 스탠 바이 상태 이상 다음 으로 진행 처리
    if(!isReadyTakeOff && CheckAllRunEngine(allPC, EEngineProgress::STANDBY_OTHER_PLAYER))
    {
        isReadyTakeOff = true;
        
        for(auto* onePC : allPC)
        {
            auto& oneData = allEngineProgData.dataMap[onePC];
            if(oneData.curProgress == EEngineProgress::STANDBY_OTHER_PLAYER)
                CheckProgress(onePC, oneData.curProgress);
        }
        return;
    }

    // 모든 pc가 이륙 대기 상태가 되면 종료 처리
    if(CheckAllRunEngine(allPC, EEngineProgress::TAKE_OFF))
    {
        ObjectiveEnd(true);
        return;
    }
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
    auto allPC = UJ_Utility::GetAllMissionPC(GetWorld());

    for(const auto* pc : allPC)
    {
        auto& data = allEngineProgData.dataMap[pc];
        // 성공 비율 계산
        float rate = data.CalcSuccessRate(data.successValue);

        totalRate += rate;
    }

    // 평균 계산
    float avg = totalRate / allPC.Num();
    // 수행도에 적용
    SUCCESS_PERCENT = avg;
}

bool AJ_ObjectiveEngineStart::CheckAllRunEngine(const TArray<class AJ_MissionPlayerController *> pcs, EEngineProgress checkType)
{
    // 해당 절차 이상인지 전부 체크
    bool isAllEnd = true;
    for(auto* onePC : pcs)
    {
        if(!onePC) continue;
        if(!allEngineProgData.dataMap.Contains(onePC)) continue;

        if(allEngineProgData.dataMap[onePC].curProgress < checkType)
        {
            isAllEnd = false;
            break;
        }
    }

    return isAllEnd;
}
#pragma endregion

#pragma region obj UI 설정 단
void AJ_ObjectiveEngineStart::SendObjUIData(class AJ_MissionPlayerController *pc, bool isInit)
{
    Super::SendObjUIData(pc, isInit);

    auto data = SetEngineUIData(pc);
    pc->objUIComp->CRPC_StartObjUIEngine(data);
}

void AJ_ObjectiveEngineStart::UpdateObjUI()
{
    if(!HasAuthority()) return;

    // 모든 pc 가져오기
    auto allPC = UJ_Utility::GetAllMissionPC(GetWorld());
    // pc에게 새 전술명령 UI 시작 srpc
    for(auto* pc : allPC)
    {
		// 과도한 crpc 방지 처리
		// 이전에 보낸 ui 데이터와 동일한지 검증
        auto orderData = SetEngineUIData(pc);

		// 다르면 업데이트
		// @@ 템플릿 쓸 수 있을지 고민
		FTacticalOrderData temp(this->orderType, orderData);
		bool canUpdate = CheckSendSameData(pc, temp);
		if(canUpdate)
		{
			// 데이터 보내기
			pc->objUIComp->CRPC_UpdateObjUIEngine(orderData);
		}
    }
}

FEngineProgressData AJ_ObjectiveEngineStart::SetEngineUIData(class AJ_MissionPlayerController *pc)
{
    // 각자 현재 절차 값
    return allEngineProgData.dataMap[pc];
}

#pragma endregion
