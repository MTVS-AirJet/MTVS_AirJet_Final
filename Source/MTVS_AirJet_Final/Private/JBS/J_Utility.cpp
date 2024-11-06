// Fill out your copyright notice in the Description page of Project Settings.


#include "JBS/J_Utility.h"
#include "JBS/J_BaseMissionPawn.h"
#include "JBS/J_MissionGameState.h"
#include "KHS/K_GameState.h"
#include "Kismet/GameplayStatics.h"
#include "Math/MathFwd.h"
#include "Math/UnrealMathUtility.h"
#include "UObject/Object.h"
#include <JBS/J_GameInstance.h>
#include <KHS/K_GameInstance.h>
#include <JBS/J_MissionGamemode.h>
#include "JBS/J_MissionPlayerController.h"
#include <Engine/World.h>

// 전역 함수
UJ_GameInstance *UJ_Utility::GetJGameInstance(const UWorld* world)
{
    auto* gi =  Cast<UJ_GameInstance>(UGameplayStatics::GetGameInstance(world));
    check(gi);

    return gi;
}

UK_GameInstance *UJ_Utility::GetKGameInstance(const UWorld* world)
{
    auto* gi =  Cast<UK_GameInstance>(UGameplayStatics::GetGameInstance(world));
    check(gi);

    return gi;
}

AJ_MissionGamemode* UJ_Utility::GetMissionGamemode(const UWorld* world)
{
    auto* gm = world->GetAuthGameMode<AJ_MissionGamemode>();
    check(gm);

    return gm;
}

AK_GameState *UJ_Utility::GetKGameState(const UWorld *world)
{
    auto* gs = world->GetGameState<AK_GameState>();
    check(gs);

    return gs;
}

AJ_MissionGameState* UJ_Utility::GetMissionGameState(const UWorld* world)
{
    auto* gs = world->GetGameState<AJ_MissionGameState>();
    check(gs);

    return gs;
}

AJ_BaseMissionPawn *UJ_Utility::GetBaseMissionPawn(const UWorld *world, int32 playerIdx)
{
    auto* pc = UGameplayStatics::GetPlayerController(world, playerIdx);
    check(pc);
    auto* player = pc->GetPawn<AJ_BaseMissionPawn>();
    check(player);

    return player;
}

TArray<AJ_MissionPlayerController *> UJ_Utility::GetAllMissionPC(const UWorld *world)
{
    auto* gs = world->GetGameState<AJ_MissionGameState>();
    check(gs);
    // 모든 pc 가져오기
    auto allPC = gs->GetAllPlayerController();

    return allPC;
}

TArray<APawn*> UJ_Utility::GetAllMissionPawn(const UWorld *world)
{
    auto* gs = world->GetGameState<AJ_MissionGameState>();
    check(gs);
    // 모든 플레이어의 폰 가져오기
    auto allPawns = gs->GetAllPlayerPawn();

    // GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString::Printf(TEXT("폰 개수 : %d"), allPawns.Num()));

    return allPawns;
}

bool UJ_Utility::CheckValueRange(float value, float min, float max, bool inClusive)
{
    bool result = inClusive ?
        min <= value && value <= max
        : min < value && value < max;

    return result;
}

FString UJ_Utility::PilotRoleToString(EPilotRole role)
{
    FString result;

    switch (role) {
    case EPilotRole::WING_COMMANDER:
        result = TEXT("편대장");
        break;
    case EPilotRole::LEFT_WINGER:
        result = TEXT("Left Winger");
        break;
    case EPilotRole::RIGHT_WINGER:
        result = TEXT("Right Winger");
        break;
    }

    return result;
}

FString UJ_Utility::TacticalOrderToString(ETacticalOrder type)
{
    FString result;

    switch(type)
    {

    case ETacticalOrder::NONE:
        result = TEXT("");
        break;
    case ETacticalOrder::MOVE_THIS_POINT:
        result = TEXT("");
        break;
    case ETacticalOrder::FORMATION_FLIGHT:
        result = TEXT("편대 비행 유지하기");
        break;
    case ETacticalOrder::NEUTRALIZE_TARGET:
        result = TEXT("지상 목표 제거하기");
        break;
    }

    return result;
}

FString UJ_Utility::ToStringBool(bool value)
{
    return value ? TEXT("TRUE") : TEXT("FALSE");
}

// === 구조체 함수 구현

ETacticalOrder FMissionObject::GetOrderType() const
{
    ETacticalOrder type;

    switch(this->commandNo)
    {
        case 0: type = ETacticalOrder::MOVE_THIS_POINT; break;
        case 1: type = ETacticalOrder::FORMATION_FLIGHT; break;
        case 2: type = ETacticalOrder::NEUTRALIZE_TARGET; break;

        default: type = ETacticalOrder::NONE; break;
    }

    return type;
}

FTransform FJVector2D::GetTransform() const
{
    // x,y 값 이랑 정해진 고도 값을 가지고 변환
    // 범위 == 5킬로 기준 -25만 ~ 25만
    float minValue = UJ_Utility::defaultMissionMapSize * -.5f;
    float maxValue = UJ_Utility::defaultMissionMapSize * .5f;
    // x,y,z 계산
    float mapX = FMath::Lerp(minValue, maxValue, this->x / 100.f);
    float mapY = FMath::Lerp(minValue, maxValue, this->y / 100.f);
    float mapZ = UJ_Utility::defaultMissionObjectHeight;
    // 위치 값
    FVector vec(mapX, mapY, mapZ);
    // @@ 회전 값 | 임시로 0,0,mapZ 를 바라보도록 | 받아와야 할지도?
    FVector targetLoc(0,0,mapZ);
    FRotator rot = (targetLoc - vec).GetSafeNormal().Rotation();

    
    FTransform tr = FTransform(rot,vec,  FVector::OneVector);

    return tr;
}

bool UJ_Utility::GetLocalPlayerController(const UWorld *world, class AJ_MissionPlayerController *&outPC)
{
    // 로컬 플레이어 찾기
    outPC = Cast<AJ_MissionPlayerController>(world->GetFirstPlayerController());

    // 유효성 체크
    return IsValid(outPC);
}

// 엔진 수행 맵 초기화
void FEngineProgressAllData::InitDataMap(const TArray<AJ_MissionPlayerController *> &pcAry)
{
    for(auto* pc : pcAry)
    {
        dataMap.Add(pc, FEngineProgressData());
    }
}

int FEngineProgressData::ConvertProgressEnumToInt(EEngineProgress type) const
{
    // uint8 범위 넘어가서 그대로 사용 불가 변환 해야함 | 2의 type -1 제곱
    int pow = static_cast<int>(type) - 1;
    int value = static_cast<int>(FMath::Pow(2.0, pow));

    return value;
}

void FEngineProgressData::AddSuccessValue(EEngineProgress type)
{
    this->successValue += ConvertProgressEnumToInt(type);
}

void FEngineProgressData::SetNextProgress()
{
    switch(this->curProgress)
    {
    case EEngineProgress::None:
        break;
    case EEngineProgress::MIC_SWITCH_ON:
        this->curProgress = EEngineProgress::ENGINE_GEN_SWITCH_ON;
        break;
    case EEngineProgress::ENGINE_GEN_SWITCH_ON:
        this->curProgress = EEngineProgress::ENGINE_CONTROL_SWITCH_ON;
        break;
    case EEngineProgress::ENGINE_CONTROL_SWITCH_ON:
        this->curProgress = EEngineProgress::JFS_STARTER_SWITCH_ON;
        break;
    case EEngineProgress::JFS_STARTER_SWITCH_ON:
        this->curProgress = EEngineProgress::ENGINE_MASTER_SWITCH_ON;
        break;
    case EEngineProgress::ENGINE_MASTER_SWITCH_ON:
        this->curProgress = EEngineProgress::JFS_HANDLE_PULL;
        break;
    case EEngineProgress::JFS_HANDLE_PULL:
        this->curProgress = EEngineProgress::ENGINE_THROTTLE_IDLE;
        break;
    case EEngineProgress::ENGINE_THROTTLE_IDLE:
        this->curProgress = EEngineProgress::CLOSE_CANOPY;
        break;
    case EEngineProgress::CLOSE_CANOPY:
        this->curProgress = EEngineProgress::STANDBY_OTHER_PLAYER;
        break;
    case EEngineProgress::STANDBY_OTHER_PLAYER:
        this->curProgress = EEngineProgress::RELEASE_SIDE_BREAK;
    }
}

float FEngineProgressData::CalcSuccessRate(int value)
{
    int cnt = 0;
    int enumCnt = 0;

    for(int i = static_cast<int>(EEngineProgress::MIC_SWITCH_ON); i <= static_cast<int>(EEngineProgress::RELEASE_SIDE_BREAK); i++)
    {
        // @@ 어떤 거 틀렸는지 반환 할 수 있게 고도화 가능
        // 수행 절차 하나하나 성공 확인
        bool isSuccess = value & i;
        if(isSuccess)
            cnt++;

        enumCnt++;
    }
    // 비율 반환
    return (float) cnt / enumCnt;
}

FString FEngineProgressData::ToStringProgressEnum(EEngineProgress type) const
{
    FString str = "";

    switch (type) {
    case EEngineProgress::None:
        str = TEXT("미설정");
        break;
    case EEngineProgress::MIC_SWITCH_ON:
        str = TEXT("통신 시스템 활성화");
        break;
    case EEngineProgress::ENGINE_GEN_SWITCH_ON:
        str = TEXT("엔진 발전기 준비 완료");
        break;
    case EEngineProgress::ENGINE_CONTROL_SWITCH_ON:
        str = TEXT("엔진 제어 장치 가동");
        break;
    case EEngineProgress::JFS_STARTER_SWITCH_ON:
        str = TEXT("JFS 스타트 준비");
        break;
    case EEngineProgress::ENGINE_MASTER_SWITCH_ON:
        str = TEXT("엔진 마스터 켜기");
        break;
    case EEngineProgress::JFS_HANDLE_PULL:
        str = TEXT("JFS 작동 시작");
        break;
    case EEngineProgress::ENGINE_THROTTLE_IDLE:
        str = TEXT("엔진 공회전 상태 유지");
        break;
    case EEngineProgress::CLOSE_CANOPY:
        str = TEXT("조종석 밀폐");
        break;
    case EEngineProgress::STANDBY_OTHER_PLAYER:
        str = TEXT("@@다른 파일럿 대기");
        break;
    case EEngineProgress::RELEASE_SIDE_BREAK:
        str = TEXT("@@사이드 브레이드 해제");
        break;
    }

    return str;
}

FString FRichString::FormatString(const FString &str, ETextStyle type)
{
    FString result = "";

    switch(type)
    {
    case ETextStyle::DEFAULT:
        result = FString::Printf(TEXT("<Default>%s</>"), *str);
        break;
    case ETextStyle::SUCCESS:
        result = FString::Printf(TEXT("<Success>%s</>"), *str);
        break;
    case ETextStyle::FAIL:
        result = FString::Printf(TEXT("<Fail>%s</>"), *str);
        break;
    }

    return result;
}

bool FEngineProgressData::CheckProgressSuccess(EEngineProgress type) const
{
    int checkType = ConvertProgressEnumToInt(type);

    return successValue & checkType;
}
