// Fill out your copyright notice in the Description page of Project Settings.


#include "JBS/J_ObjectiveFormationFlight.h"
#include "Engine/Engine.h"
#include "GameFramework/Pawn.h"
#include "Internationalization/Text.h"
#include "JBS/J_MissionPlayerController.h"
#include "JBS/J_Utility.h"
#include "JBS/J_BaseMissionPawn.h"
#include "JBS/J_ObjectiveUIComp.h"
#include "TimerManager.h"
#include "UObject/Class.h"
#include <algorithm>

void AJ_ObjectiveFormationFlight::BeginPlay()
{
    Super::BeginPlay();

    // 목표 활성화 될때 모든 플레이어 목록 가져오기
    this->objectiveActiveDel.AddLambda([this]()
    {
        allPawns = UJ_Utility::GetAllMissionPawn(GetWorld());
    });

    // 목표 완료시 목표 UI 완료 바인드
    objectiveEndDel.AddUObject(this, &AJ_ObjectiveFormationFlight::SRPC_EndObjUI);
}

void AJ_ObjectiveFormationFlight::Tick(float deltaTime)
{
    Super::Tick(deltaTime);

    // 목표 활성화 체크
    if(!IS_OBJECTIVE_ACTIVE) return;

    // 시간 흐르기
    progressTime += deltaTime;
    // 편대 조건 충족 시
    if(IS_FORMATION)
        formationTime += deltaTime;

    // 수행도 율 갱신
    SetSuccessPercent(formationTime / progressTime);

    // 편대 조건 체크
    IS_FORMATION = CheckFormation();
    // GEngine->AddOnScreenDebugMessage(-1, -1.f, FColor::White, FString::Printf(TEXT("체크리스트 값 : %d"), checklistValue));
    // FString check1 = checklistValue & 1 ? TEXT("고도 맞음") : TEXT("고도 아냐");
    // FString check2 = checklistValue & 2 ? TEXT("진형 맞아") : TEXT("진형 아냐");
    
    // FString checkStr = FString::Printf(TEXT("체크리스트 값 : %s, %s\n수행도 값 : %.2f\n현재 편대 비행 중 : %s"), *check1, *check2, SUCCESS_PERCENT, IS_FORMATION ? TEXT("맞아") : TEXT("아냐"));

    // GEngine->AddOnScreenDebugMessage(-1, -1.f, FColor::White, FString::Printf(TEXT("%s"), *checkStr));

}

bool AJ_ObjectiveFormationFlight::CheckFormation()
{
    // 체크 초기화
    checklistValue = 0;
    // 체크리스트 실행
    checklistValue = static_cast<int>(
        CheckBase(EFormationChecklist::FLIGHT_HEIGHT)
        | CheckBase(EFormationChecklist::ALIGN_FORMATION)
    );

    return checklistValue == 3;
}

int AJ_ObjectiveFormationFlight::CheckBase(EFormationChecklist checkType)
{
    bool isPass = true;
    // FIXME 나중에 체크 컴포넌트 붙여서 플레이어 각각 계산한 것을 취합하도록
    switch(checkType)
    {
        case EFormationChecklist::FLIGHT_HEIGHT:
            isPass = CheckHeight();
            break;
        case EFormationChecklist::ALIGN_FORMATION:
            isPass = CheckAlign();
            break;
    }

    // 체크된 값 주기
    return isPass ? static_cast<int>(checkType) : 0;
}

bool AJ_ObjectiveFormationFlight::CheckHeight()
{
    // FIXME 나중에 체크 컴포넌트 붙여서 플레이어 각각 계산한 것을 취합하도록
    bool isPass = true;
    for(auto* pawn : allPawns)
    {
        // 고도 확인
        // 이거 비교값을 보정시키고 싶음
        float pawnHeight = pawn->GetActorLocation().Z;
        isPass = UJ_Utility::CheckValueRange(pawnHeight, checkHeight - checkHeightAdj, checkHeight + checkHeightAdj);
    }

    return isPass;
}

bool AJ_ObjectiveFormationFlight::CheckAlign()
{
    bool isPass = true;

    // solved 나중에 체크 컴포넌트 붙여서 플레이어 각각 계산한 것을 취합하도록
    TArray<bool> passList;
    
    allPawns = UJ_Utility::GetAllMissionPawn(GetWorld());

    for(auto* pawn : allPawns)
    {
        // pc 가져오기
        auto* pc = pawn->GetController<AJ_MissionPlayerController>();
        check(pc);

        // 다른 폰 가져오기
        for(auto* otherPawn : allPawns) 
        {
            if(pawn == otherPawn) continue;

            // 플레이어 역할에 따라 다른 위치에 있도록 요구
            // 리더 == 내 앞에 다른 팀원 없음
            // lt 윙어 == 내 왼쪽에 아무도 없음
            // rt 윙어 == 내 오른쪽에 아무도 없음

            bool subPass = true;
            switch(pc->pilotRole) 
            {
                // 나를 제외한 다른 폰들과 내적
                case EPilotRole::WING_COMMANDER:
                {
                    // 앞에 뭔가 있으면 false
                    subPass = !CheckForward(pawn, otherPawn);
                    // GEngine->AddOnScreenDebugMessage(-1, -1.f, FColor::Purple, FString::Printf(TEXT("전방 존재 확인 %s"), subPass ? TEXT("TRUE"): TEXT("false")));
                }
                    break;
                // 외적
                case EPilotRole::LEFT_WINGER:
                {
                    subPass = CheckLeftRight(pawn, otherPawn);
                    // GEngine->AddOnScreenDebugMessage(-1, -1.f, FColor::Purple, FString::Printf(TEXT("왼쪽 확인 %s"), subPass ? TEXT("TRUE"): TEXT("false")));
                }
                    break;
                case EPilotRole::RIGHT_WINGER:
                {
                    subPass = CheckLeftRight(pawn, otherPawn, false);
                }
                    break;
            }
            
            passList.Add(subPass);
        }
    }

    // 패스 리스트 중 false 하나 라도 있으면 false;
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
                        : cross.Z < 0;

    return isPass;
}

// void AJ_ObjectiveFormationFlight::SRPC_EndSubObjUI()
// {
//     Super::SRPC_EndSubObjUI();

    
// }

void AJ_ObjectiveFormationFlight::SRPC_EndObjUI()
{
    Super::SRPC_EndObjUI();
}

void AJ_ObjectiveFormationFlight::SRPC_StartNewObjUI()
{
    if(!HasAuthority() || !IS_OBJECTIVE_ACTIVE) return;

	// 모든 pc 가져오기
	auto allPC = UJ_Utility::GetAllMissionPC(GetWorld());

    // pc에게 새 전술명령 UI 시작 crpc
    for(auto* pc : allPC)
    {
		// 보낼 목표 데이터 구성
		auto orderData = SetFormationUIData(pc);
		// ui 생성 시작
        pc->objUIComp->CRPC_StartObjUIFormation(orderData);
    }

    // FIXME
    PlayCommander(12);
    FTimerHandle timerHandle3;
    GetWorld()->GetTimerManager()
        .SetTimer(timerHandle3, [this]() mutable
    {
        //타이머에서 할 거
        PlayCommander(13);
    }, 3.f, false);

    FTimerHandle timerHandle2;
    GetWorld()->GetTimerManager()
        .SetTimer(timerHandle2, [this]() mutable
    {
        //타이머에서 할 거
        PlayCommander(14);

    }, 7.f, false);
}

void AJ_ObjectiveFormationFlight::SRPC_UpdateObjUI()
{
    if(!HasAuthority() || !IS_OBJECTIVE_ACTIVE) return;

	// 보낼 데이터
    // 모든 pc 가져오기
    auto allPC = UJ_Utility::GetAllMissionPC(GetWorld());

    // pc에게 새 전술명령 UI 시작 srpc
    for(auto* pc : allPC)
    {
		auto orderData = SetFormationUIData(pc);
    
        FTacticalOrderData tempData(this->orderType, orderData);

		
		// 과도한 crpc 방지 처리
		if(!prevObjUIDataMap.Contains(pc) || tempData != prevObjUIDataMap[pc])
		{
			// 데이터 보내기
			pc->objUIComp->CRPC_UpdateObjUIFormation(orderData);
			// objui데이터 맵에 저장
			prevObjUIDataMap.Add(pc, tempData);
		}
    }
}

FFormationFlightUIData AJ_ObjectiveFormationFlight::SetFormationUIData(class AJ_MissionPlayerController *pc)
{
    return FFormationFlightUIData(
        isFormation
        , this->checkHeight
        , pc->GetPawn()->GetActorLocation().Z
        , pc->pilotRole
        , checklistValue >= static_cast<int>(EFormationChecklist::ALIGN_FORMATION));
}


// 최초에 설정만
FTacticalOrderData AJ_ObjectiveFormationFlight::SetObjUIData(AJ_MissionPlayerController* pc)
{
    FFormationFlightUIData data(
        isFormation
        , this->checkHeight
        , pc->GetPawn()->GetActorLocation().Z
        , pc->pilotRole
        , checklistValue >= static_cast<int>(EFormationChecklist::ALIGN_FORMATION));

    // 보낼 목표 데이터
    return FTacticalOrderData(this->orderType, data);
}

void AJ_ObjectiveFormationFlight::OnCheckCapsuleBeginOverlap(UPrimitiveComponent *OverlappedComponent,
                                                             AActor *OtherActor, UPrimitiveComponent *OtherComp,
                                                             int32 OtherBodyIndex, bool bFromSweep,
                                                             const FHitResult &SweepResult)
{
    // 충돌한게 미션 폰이면 목표 성공 처리
    if(OtherActor->IsA<AJ_BaseMissionPawn>())
    {
        // GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::White, FString::Printf(TEXT("충돌한 플레이어 : %s"), *OtherActor->GetName()));
        // GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::White, TEXT("이동 목표 성공"));
        // 수행도 1
        // SUCCESS_PERCENT = 1.f;
        this->ObjectiveEnd(true);
    }   
}

void AJ_ObjectiveFormationFlight::ObjectiveActive()
{
    Super::ObjectiveActive();

    // 목표 ui 신규 갱신
    SRPC_StartNewObjUI();
}

