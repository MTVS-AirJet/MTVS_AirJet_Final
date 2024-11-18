// Fill out your copyright notice in the Description page of Project Settings.


#include "JBS/J_ObjectiveFormationFlight.h"
#include "Engine/Engine.h"
#include "GameFramework/Pawn.h"
#include "Internationalization/Text.h"
#include "JBS/J_BaseMissionObjective.h"
#include "JBS/J_MissionPlayerController.h"
#include "JBS/J_Utility.h"
#include "JBS/J_BaseMissionPawn.h"
#include "JBS/J_ObjectiveUIComp.h"
#include "TimerManager.h"
#include "UObject/Class.h"
#include <algorithm>

#pragma region 시작 설정 단
/* icon UI 캐시 -> (서버단) 딜리게이트 바인드 -> init 목표 초기 설정 -> set active(false)
*/

void AJ_ObjectiveFormationFlight::BeginPlay()
{
    Super::BeginPlay();

}

void AJ_ObjectiveFormationFlight::InitBindDel()
{
    // movepoint 가 아닌 베이스 바인드 사용 | movepoint는 ui 갱신 안함
    AJ_BaseMissionObjective::InitBindDel();    

    // 목표 완료시 수행도 갱신 바인드
    this->objectiveEndDel.AddLambda([this]{
        this->SetSuccessPercent(formationTime / progressTime)        ;
    });
}

void AJ_ObjectiveFormationFlight::ObjectiveActive()
{
    
    Super::ObjectiveActive();

    // 목표 ui 신규 갱신
    StartNewObjUI();
}



#pragma endregion

#pragma region 목표 진행 단
/* tick 목표 활성화시 실패 판정 체크 | CheckFail, ObjectiveEnd
-> 체크 캡슐에 충돌시 성공 처리 | OnCheckCapsuleBeginOverlap, ObjectiveEnd // 부모 movepoint 작동

-> 목표 활성화 중 편대 조건 체크 1,2 | CheckFormation
-> 조건 타입에 따라 다른 조건 함수 실행 | CheckBase
-> 고도 체크 | CheckHeight
-> 진형 체크 | CheckAlign -> 플레이어 역할에 따라 다른 조건 함수 실행 | CheckForward, CheckLeftRight
*/
void AJ_ObjectiveFormationFlight::Tick(float deltaTime)
{
    Super::Tick(deltaTime);

    // 목표 활성화 체크
    if(IS_OBJ_ENDED) return;

    // 편대 조건 체크
    IS_FORMATION = CheckFormation();

    // 시간 흐르기
    progressTime += deltaTime;
    checkInterval -= deltaTime;

    // 편대 조건 충족 시 충족 시간 가산
    if(IS_FORMATION)
        formationTime += deltaTime;

    // // @@ 갱신 시간 체크 | ui에 표기하는 것도 아닌데 마지막에 한 번만 하면 될듯?
    // if(checkInterval <= 0)
    // {
    //     checkInterval = 5.f;
    //     // 수행도 율 갱신
    //     SetSuccessPercent(formationTime / progressTime);
    // }

    // GEngine->AddOnScreenDebugMessage(-1, -1.f, FColor::White, FString::Printf(TEXT("체크리스트 값 : %d"), checklistValue));
    // FString check1 = checklistValue & 1 ? TEXT("고도 맞음") : TEXT("고도 아냐");
    // FString check2 = checklistValue & 2 ? TEXT("진형 맞아") : TEXT("진형 아냐");
    
    // FString checkStr = FString::Printf(TEXT("체크리스트 값 : %s, %s\n수행도 값 : %.2f\n현재 편대 비행 중 : %s"), *check1, *check2, SUCCESS_PERCENT, IS_FORMATION ? TEXT("맞아") : TEXT("아냐"));

    // GEngine->AddOnScreenDebugMessage(-1, -1.f, FColor::White, FString::Printf(TEXT("%s"), *checkStr));
}

bool AJ_ObjectiveFormationFlight::CheckFormation()
{
    // 체크리스트 실행 | 비트 마스크 체크
    checklistValue = static_cast<int>(
        CheckBase(EFormationChecklist::FLIGHT_HEIGHT)
        | CheckBase(EFormationChecklist::ALIGN_FORMATION)
    );

    // 통과 비트마스크 값
    int passValue = static_cast<int>(EFormationChecklist::FLIGHT_HEIGHT)
                    + static_cast<int>(EFormationChecklist::ALIGN_FORMATION);

    // 값 반환
    return checklistValue == passValue;
}

int AJ_ObjectiveFormationFlight::CheckBase(EFormationChecklist checkType)
{
    bool isPass = true;
    switch(checkType)
    {
        case EFormationChecklist::FLIGHT_HEIGHT:
            isPass = CheckHeight();
            break;
        case EFormationChecklist::ALIGN_FORMATION:
            isPass = CheckAlign();
            break;
    }

    // 체크된 값 반환
    return isPass ? static_cast<int>(checkType) : 0;
}

bool AJ_ObjectiveFormationFlight::CheckHeight()
{
    bool isPass = true;

    auto allPawns = UJ_Utility::GetAllMissionPawn(GetWorld());
    for(auto* pawn : allPawns)
    {
        if(!pawn) continue;

        // 고도 확인
        float pawnHeight = pawn->GetActorLocation().Z;
        float minCheckHeight = checkHeight - checkHeightAdj;
        float maxCheckHeight = checkHeight + checkHeightAdj;

        // 기준 고도 및 오차 범위 고려하여 계산 | 기준 , - 범위, + 범위
        isPass = UJ_Utility::CheckValueRange(pawnHeight, minCheckHeight, maxCheckHeight);
    }

    return isPass;
}

bool AJ_ObjectiveFormationFlight::CheckAlign()
{
    bool isPass = true;
    TArray<bool> passList;
    // 모든 pc 가져오기
    auto allPC = UJ_Utility::GetAllMissionPC(GetWorld());
    auto allPawns = UJ_Utility::GetAllMissionPawn(GetWorld());

    // 모든 플레이어 검사
    for(auto* pc : allPC)
    {
        if(!pc) continue;
        
        auto* curPawn = pc->GetPawn<AJ_BaseMissionPawn>();
        if(!curPawn) continue;

        // 다른 폰과 상대 위치 검사
        for(auto* otherPawn : allPawns)
        {
            if(curPawn == otherPawn) continue;

            // 플레이어 역할은 gamemode 전술명령 시작시 설정됨
            // 플레이어 역할에 따라 다른 위치에 있도록 요구
            // 리더 == 내 앞에 다른 팀원 없음
            // lt 윙어 == 내 왼쪽에 아무도 없음
            // rt 윙어 == 내 오른쪽에 아무도 없음


            // @@ 현재 리더, lt 만 사용 ( 2명이라 )
            // @@ 나중에 리더, rt로 변경
            bool subPass = true;
            // 역할에 따라 다른 조건 검사
            switch(pc->pilotRole) 
            {
                // 나를 제외한 다른 폰들과 내적
                case EPilotRole::WING_COMMANDER:
                    // 앞에 뭔가 있으면 false
                    subPass = !CheckForward(curPawn, otherPawn);
                    break;
                // 외적
                case EPilotRole::LEFT_WINGER:
                    subPass = CheckLeftRight(curPawn, otherPawn);
                    break;
                case EPilotRole::RIGHT_WINGER:
                    subPass = CheckLeftRight(curPawn, otherPawn, false);
                    break;
            }
            // 체크 결과 추가
            passList.Add(subPass);
        }
    }

    // 패스 리스트 중 false 하나 라도 있으면 실패 처리
    isPass = std::all_of(passList.begin(), passList.end(), [](bool subPass){
        return subPass;
    });
    
    return isPass;
}

bool AJ_ObjectiveFormationFlight::CheckForward(AActor* actor, AActor* otherActor)
{
    // 해당 플레이어로의 방향
    FVector toOtherPawn = (otherActor->GetActorLocation() - actor->GetActorLocation()).GetSafeNormal();

    // 내적
    float dot = FVector::DotProduct(actor->GetActorForwardVector(), toOtherPawn);
    // 전방 확인 == 양수일때
    bool isPass = dot > 0;

    return isPass;
}

bool AJ_ObjectiveFormationFlight::CheckLeftRight(AActor *actor, AActor* otherActor, bool checkLeft)
{
    // 해당 플레이어로의 방향
    FVector toOtherPawn = (otherActor->GetActorLocation() - actor->GetActorLocation()).GetSafeNormal();
    
    // 외적
    FVector cross = FVector::CrossProduct(actor->GetActorForwardVector(), toOtherPawn);
    // 기준에 따라 왼쪽 오른쪽 확인
    bool isPass = checkLeft ? cross.Z > 0
                        : cross.Z <= 0;

    return isPass;
}

void AJ_ObjectiveFormationFlight::OnCheckCapsuleBeginOverlap(
    UPrimitiveComponent *OverlappedComponent,
    AActor *OtherActor, UPrimitiveComponent *OtherComp,
    int32 OtherBodyIndex, bool bFromSweep,
    const FHitResult &SweepResult)
{
    // 서버단 및 활성화 단계일때만 작동
    if(!HasAuthority()) return;
    if(IS_OBJ_ENDED) return;

    // 충돌한게 미션 폰이면 목표 성공 처리
    if(OtherActor->IsA<AJ_BaseMissionPawn>())
        ObjectiveEnd(true);
}



#pragma endregion

#pragma region 목표 UI 적용 단
/* 특정 타이밍에 목표 UI 시작 | StartNewObjUI
-> 각 pc마다 목표 ui 데이터 설정 | SendObjUIData, SetFormationUIData, CRPC_StartObjUIFormation
-> 특정 갱신 타이밍 마다 업데이트 UI | UpdateObjUI -> 이전에 보냈는지 체크후 업데이트 | CheckSendSameData
-> 목표 종료 나 서브 목표 종료시 ui 애니메이션 실행 요청 | EndObjUI , EndSubObjUI
-> 목표 종료시 비작동*/

void AJ_ObjectiveFormationFlight::SendObjUIData(AJ_MissionPlayerController *pc, bool isInit)
{
    Super::SendObjUIData(pc, isInit);

    auto data = SetFormationUIData(pc);
    pc->objUIComp->CRPC_StartObjUIFormation(data);
}

FFormationFlightUIData AJ_ObjectiveFormationFlight::SetFormationUIData(AJ_MissionPlayerController *pc)
{
    return FFormationFlightUIData(
        isFormation
        , this->checkHeight
        , pc->GetPawn()->GetActorLocation().Z
        , pc->pilotRole
        , checklistValue >= static_cast<int>(EFormationChecklist::ALIGN_FORMATION));
}

void AJ_ObjectiveFormationFlight::UpdateObjUI()
{
    if(!HasAuthority()) return;

    // 모든 pc 가져오기
    auto allPC = UJ_Utility::GetAllMissionPC(GetWorld());
    // pc에게 새 전술명령 UI 시작 srpc
    for(auto* pc : allPC)
    {
        if(!pc) continue;

		// 과도한 crpc 방지 처리
		// 이전에 보낸 ui 데이터와 동일한지 검증
        auto orderData = SetFormationUIData(pc);

		// 다르면 업데이트
		// @@ 템플릿 쓸 수 있을지 고민
		FTacticalOrderData temp(this->orderType, orderData);
		bool canUpdate = CheckSendSameData(pc, temp);
		if(canUpdate)
		{
			// 데이터 보내기
			pc->objUIComp->CRPC_UpdateObjUIFormation(orderData);
		}
    }
}

#pragma endregion


