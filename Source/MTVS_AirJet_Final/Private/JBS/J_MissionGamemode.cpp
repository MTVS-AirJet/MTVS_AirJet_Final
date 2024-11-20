// Fill out your copyright notice in the Description page of Project Settings.


#include "JBS/J_MissionGamemode.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/Engine.h"
#include "Engine/LatentActionManager.h"
#include "GameFramework/PlayerController.h"
#include "JBS/J_GameInstance.h"
#include "JBS/J_MissionGameState.h"
#include "JBS/J_MissionPlayerController.h"
#include "JBS/J_MissionStartPointActor.h"
#include "JBS/J_Utility.h"
#include "KHS/K_CesiumManager.h"
#include "KHS/K_CesiumTeleportBox.h"
#include "KHS/K_GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "LHJ/L_Viper.h"
#include "Math/MathFwd.h"
#include "JBS/J_ObjectiveManagerComponent.h"
#include "Templates/Casts.h"
#include "TimerManager.h"
#include <JBS/J_MissionSpawnPointActor.h>
#include <Engine/World.h>

#pragma region 안쓰는 무덤
// void AJ_MissionGamemode::LoadMissionMap()
// {
//     // 미션 지역 로드
//     UGameplayStatics::LoadStreamLevelBySoftObjectPtr(GetWorld(), missionMapPrefab, true, true, FLatentActionInfo());
// }

#pragma endregion

#pragma region 레벨 시작 단
/*
레벨 시작 시 목표 매니저 컴포넌트 캐시 | BeginPlay
-> 미션 시작 | StartMission
-> 미션 데이터 호스트 게임 인스턴스 에서 가져오기 | LoadMissionData
-> 미션 시작 지점 포인트 액터 생성 | InitMissionStartPoint
-> 전술명령 생성 및 초기화 | InitDefaultObj, InitObjectiveList
-> 플레이어 시작 위치 설정 | SetSpawnPoints // 지휘관 있었을때의 영향으로 역할에 따라 분류되서 가져옴
-> 다른 액터들이 준비되도록 .1초 타이머 이후 세슘 액터 캐시 | CacheCesiumActors
*/
void AJ_MissionGamemode::BeginPlay()
{
    Super::BeginPlay();

    // 목표 매니저 컴포넌트 가져오기 | 생성자에서 생성하는게 안되서 블프에 넣음
    objectiveManagerComp = this->GetComponentByClass<UJ_ObjectiveManagerComponent>();
    
    // 미션 시작
    StartMission();
}

void AJ_MissionGamemode::StartMission()
{
    // 디버그용 더미 미션 데이터 사용 여부
    if(enableUsingDummyMissionData)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("현재 디버그용 더미 미션 데이터 사용 중"));
        curMissionData = dummyMissionData;
    }
    // GI 미션 데이터 사용
    else 
    {
        curMissionData = LoadMissionData();
    }

    // 첫 미션 데이터 가져오기
    const auto& missions = curMissionData.mission;
    const FMissionObject& firstMission = missions.Num() > 0 ? missions[0] : FMissionObject();

    // 시작 지점 액터 생성 | 첫 미션을 바라보도록 생성
    InitMissionStartPoint(curMissionData.startPoint, firstMission);

    // 목표 매니저 컴포넌트 존재 확인
    check(objectiveManagerComp);

    // FIXME 여기부터 다시 리팩토링 시작

    // 시동 및 이륙 목표 추가
    objectiveManagerComp->InitDefaultObj();
    // 전술 명령 목표 추가
    objectiveManagerComp->InitObjectiveList(curMissionData.mission);

    // 플레이어 스폰포인트 가져와서 설정
    SetSpawnPoints(spawnPoints);

    // 모든 액터가 준비된 시점에 호출
    FTimerHandle timerHandle;
    GetWorld()->GetTimerManager()
        .SetTimer(timerHandle, [this]() mutable
    {
        //타이머에서 할 거
        // 세슘 관련 액터 캐시
        CacheCesiumActors();
    }, .1f, false);
}

FMissionDataRes AJ_MissionGamemode::LoadMissionData()
{
    // 호스트 gi의 미션 데이터 가져오기 | 설정은 세션 단에서 함
    const auto* gi = UJ_Utility::GetKGameInstance(GetWorld());
    const FMissionDataRes& resultData = gi->MissionData;

    // 잘못된 미션 데이터 일 경우 더미 데이터 사용
    bool isValid = !resultData.mapName.IsEmpty();
    if(isValid)
    {
        FString debugStr = TEXT("잘못된 미션 데이터 | 더미 데이터 사용");
        UJ_Utility::PrintFullLog(debugStr, 5.f, FColor::Red);
    }
    
    return isValid ? resultData : dummyMissionData;
}

void AJ_MissionGamemode::InitMissionStartPoint(const FMissionStartPos &startPointData, const FMissionObject& firstMission)
{
    // 첫 미션이 존재하면 그걸 바라보게, 아니면 맵 중앙 바라보도록 위치 가져오기
    const FTransform& spawnTR = firstMission.GetOrderType() != ETacticalOrder::NONE 
        ? startPointData.GetTransform(firstMission.GetTransform().GetLocation())
        : startPointData.GetTransform();

    // 액터 생성
    auto* spActor = GetWorld()->
        SpawnActor<AJ_MissionStartPointActor>(startPointActorPrefab, spawnTR);
    if(!spActor) return;

    // 캐시
    START_POINT_ACTOR = spActor;
}

void AJ_MissionGamemode::SetSpawnPoints(FMissionPlayerSpawnPoints& spawnPointsStruct)
{
    // 레벨에 이미 배치된 스폰 포인트 찾기
    TArray<AActor*> outActors;
    UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), AJ_MissionSpawnPointActor::StaticClass(), spawnPointsStruct.spawnPointTag, outActors);
    // 클래스 변환
    TArray<AJ_MissionSpawnPointActor*> spPointActors;
    Algo::Transform(outActors, spPointActors, [](AActor* temp){
        return Cast<AJ_MissionSpawnPointActor>(temp);
    });
    //캐스트 후
    for(auto* spPoint : spPointActors)
    {
        // 맵에 추가
        spawnPointsStruct.spawnPointMap[spPoint->spawnType] = spPoint;
    }
}

void AJ_MissionGamemode::CacheCesiumActors()
{
    // 텔포 박스 가져오기
    cesiumTPBox = Cast<AK_CesiumTeleportBox>(
        UGameplayStatics::GetActorOfClass(GetWorld(), AK_CesiumTeleportBox::StaticClass()));
    if(!cesiumTPBox) return;

    // 정점 액터 가져오기
    cesiumZeroPos = Cast<AK_CesiumManager>(
        UGameplayStatics::GetActorOfClass(GetWorld(),
         AK_CesiumManager::StaticClass()));
    if(!cesiumZeroPos) return;

    // GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("무사히 세슘 관련 액터 캐시 성공"));
}
#pragma endregion

#pragma region 플레이어 접속 단
/*
플레이어 접속 | PostLogin
-> 호스트 pc 시작 버튼에 미션 시작 함수 바인드 | PostLogin
-> 전투기 생성 요청 | pc->SpawnMyPlayer
-> 게임 인스턴스에서 자기 역할에 맞는 플레이어 프리팹 가져오기 | pc->gi->GetMissionPlayerPrefab
-> 자기 역할에 맞는 스폰위치 가져오기 | GetPlayerSpawnTransfrom // 무조건 파일럿
-> 플레이어 생성 후 포제스 | SpawnActor, Possess
-> 역할에 따라 마우스 커서 및 입력 처리

// @@ 미션 시작 버튼 누르면 팝업 UI 시작 | ???
-> 팝업 UI 종료시 미션 시작 | StartMissionLevel
-> 미션 시작시 시동 절차 목표 활성화 | StartDefualtObj
*/
void AJ_MissionGamemode::PostLogin(APlayerController *newPlayer)
{
    Super::PostLogin(newPlayer);

    // 현재 주요 스폰 시스템
    if(enableUseOldSpawnSystem)
    {
        // 역할 설정후 플레이어 스폰
        auto* pc = CastChecked<AJ_MissionPlayerController>(newPlayer);
        pc->SpawnMyPlayer();

        // 미션 보이스 로드 하라고 요청
        pc->CRPC_ReqMissionVoiceData();
        
        // 호스트 시작 버튼에 미션 시작 함수 딜리게이트
        // @@ 나중엔 챕터 UI 시작으로 변경
        if(HasAuthority() && newPlayer->IsLocalController())
            pc->StartGameDel_Mission.AddUObject(this, &AJ_MissionGamemode::StartMissionLevel);
    }
}

// 산개 로직 적용해서 스폰 위치 계산
FTransform AJ_MissionGamemode::GetPlayerSpawnTransfrom(EPlayerRole role, AJ_MissionPlayerController* pc)
{
    // 해당 역할의 스폰 위치 계산
    auto tr = this->GetSpawnPoint(role)->GetActorTransform();
    // 몇 번째 pc 인지 받아서 시작 위치에서 뒤로 - * i 만큼 계산

    // 시작 지점의 뒤 벡터 구하기
    FVector backVector = -1 * tr.GetUnitAxis(EAxis::X);
    backVector *= spawnInterval * spawnCnt;

    // 위치 결정
    tr.SetLocation(tr.GetLocation() + backVector);

    // 스폰 카운트
    spawnCnt++;

    return tr;
}

void AJ_MissionGamemode::StartMissionLevel()
{
    // FIXME 챕터 UI 활성화할때 재생으로 옮겨야함
    auto allPC = UJ_Utility::GetAllMissionPC(GetWorld());
    for(auto* pc : allPC)
    {
        if(!pc) continue;

        pc->CRPC_PlayCommanderVoice3(static_cast<int>(EMissionProcess::MISSION_START));
    }
    

    // 시동 목표 시작
    objectiveManagerComp->StartDefualtObj();
}







void AJ_MissionGamemode::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
}



// 있으면 그거 주고 없으면 찾기 
AJ_MissionSpawnPointActor* AJ_MissionGamemode::GetSpawnPoint(EPlayerRole role)
{
    auto* spPos = spawnPoints.spawnPointMap[role];
    if(!spPos)
    {
        // 없으면 월드에서 다시 가져와서 설정
        SetSpawnPoints(spawnPoints);
        spPos = spawnPoints.spawnPointMap[role];
        // 그래도 없으면 생성하기
        if(!spPos)
            spPos = AddSpawnPoint(spawnPoints, role);
    }

    return spPos;
}



AJ_MissionSpawnPointActor* AJ_MissionGamemode::AddSpawnPoint(FMissionPlayerSpawnPoints& spawnPointsStruct, EPlayerRole addRole)
{
    // 해당 역할을 가진 스폰 포인트 생성
    auto* spPoint = GetWorld()->SpawnActor<AJ_MissionSpawnPointActor>(spawnPointsStruct.spawnPointPrefab, spawnPointsStruct.spawnPointDefaultPos[addRole]);

    spPoint->spawnType = addRole;

    // 맵에 추가
    spawnPointsStruct.spawnPointMap[addRole] = spPoint;

    return spPoint;
}







void AJ_MissionGamemode::TeleportAllStartPoint(AJ_MissionStartPointActor *startPoint)
{
    if(!HasAuthority()) return;

    // 모든 플레이어의 폰 가져오기
    auto allPawns = UJ_Utility::GetAllMissionPawn(GetWorld());

    // XXX 로딩 ui 제거 비활성
    // removeLUIDel.Broadcast();

    auto allPC = UJ_Utility::GetAllMissionPC(GetWorld());

    // 시작 지점으로 위치 이동
    for(int i = 0; i < allPC.Num(); i++)
    {

        auto* pawn = allPawns[i];
        // 역할 설정
        auto* pc = allPC[i];
        // pc 로딩 ui 잇으면 제거
        pc->CRPC_RemoveLoadingUI();
        // idx 0,1,2 를 각각 리더, lt 윙맨, rt 윙맨으로 설정
        pc->pilotRole = static_cast<EPilotRole>(i % 3);

        // 순서에 따른 위치 조정 (산개용)
        FTransform newTR = CalcTeleportTransform(pc->pilotRole);
        // 스케일 조정되지 않도록 변경
        newTR = FTransform(newTR.GetRotation(), newTR.GetLocation(), pawn->GetActorScale());
        pc->MRPC_TeleportStartPoint(newTR);
    }
}

AJ_MissionStartPointActor *AJ_MissionGamemode::GetStartPointActor()
{
    // 없으면 그냥 맵 중앙으로 설정
    if(startPointActor == nullptr)
    {
        GEngine->AddOnScreenDebugMessage(-1, 13.f, FColor::Red, TEXT("미션 시작 지점 미설정 | 기본 위치 적용"));

        FMissionStartPos& curSPData = curMissionData.startPoint;
        InitMissionStartPoint(curSPData);
    }


    return startPointActor;
}

FTransform AJ_MissionGamemode::CalcTeleportTransform(int idx)
{
    return CalcTeleportTransform(static_cast<EPilotRole>(idx % 3));
}

FTransform AJ_MissionGamemode::CalcTeleportTransform(EPilotRole role)
{
    // 시작 지점 트랜스폼
    FTransform baseTR = START_POINT_ACTOR->GetActorTransform();
    if(baseTR.Equals(FTransform::Identity))
        return baseTR;

    FVector forV = START_POINT_ACTOR->GetActorForwardVector();
    FVector rtV = START_POINT_ACTOR->GetActorRightVector();
    // 산개 로직 
    /*def triangle_vertices(h, BC, AB):
    A = (0, h)
    B = (-BC / 2, -h)
    C = (BC / 2, -h)
    return A, B, C

    A, B, C = triangle_vertices(200, 1220, 681) */
    FVector addVec;
    float& forValue = formationData.height;
    float& rtValue = formationData.baseLength;

    switch (role) {
    // 시작 지점 + 길이
    case EPilotRole::WING_COMMANDER:
        addVec = forV * forValue;
        break;
    case EPilotRole::LEFT_WINGER:
        addVec = rtV * (-1 * rtValue / 2) + forV * -1 * forValue;
        break;
    case EPilotRole::RIGHT_WINGER:
        addVec = rtV * (rtValue / 2) + forV * -1 * forValue;
        break;
    }

    // GEngine->AddOnScreenDebugMessage(-1, 333.f, FColor::White, FString::Printf(TEXT("시작 지점 %s\n변경된 지점 %s")
    //     , *baseTR.GetLocation().ToString()
    //     , *(baseTR.GetLocation() + addVec).ToString()));

    baseTR.SetLocation(baseTR.GetLocation() + addVec);

    return baseTR;
}



// 이륙 체크 및 미션 시작 단
bool AJ_MissionGamemode::AddFlightedPC(class AJ_MissionPlayerController *pc, bool isSuccess)
{
    // GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("이륙 추가 : %s"), *pc->GetName()));
    // 이륙 배열에 추가
    if(flightedPCAry.Contains(pc)) return false;
    flightedPCAry.Add(pc);

    // 이륙 딜리게이트 실행
    onePilotTakeOffDel.ExecuteIfBound(pc, isSuccess);

    FTimerHandle timerHandle;
    GetWorld()->GetTimerManager()
        .SetTimer(timerHandle, [this,pc]() mutable
    {
        if(isStartTO) return;
        //타이머에서 할 거
        // 해당 pc에게 로딩 UI 추가
        pc->CRPC_AddLoadingUI();
    }, 1.5f, false);

    
    
    // 배열 크기가 플레이어 수와 같아지면 시작 지점 텔포 및 미션 시작
    isTPReady = flightedPCAry.Num() == GetGameState<AJ_MissionGameState>()->GetAllPlayerController().Num();
    if(isTPReady)
        StartTacticalOrder();

    

    return false;
}



void AJ_MissionGamemode::SRPC_RemoveLoadingUIByPC_Implementation(class AJ_MissionPlayerController *missionPC)
{
    
}





void AJ_MissionGamemode::StartTacticalOrder()
{
    if(isStartTO) return;

    // solved 기본 목표 종료 처리
    startTODel.Broadcast(true);
    isStartTO = true;

    // GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::White, TEXT("전부 이륙"));
    

    // @@ 임시로 조금 늦게 텔포
    FTimerHandle timerHandle;
    GetWorld()->GetTimerManager()
        .SetTimer(timerHandle, [this]() mutable
    {
        // 미션 영역 변경
        cesiumTPBox->SetDestinationLogitudeLatitude(curMissionData.longitude, curMissionData.latitude);
        cesiumTPBox->MRPC_ChangeMissionArea();
        //텔포박스 비활성화
        auto* boxComp = cesiumTPBox->GetComponentByClass<UBoxComponent>();
        if(boxComp)
            boxComp->SetCollisionProfileName(FName(TEXT("NoCollision")));
        // 텔포 박스 제거
        cesiumTPBox->Destroy();

        //타이머에서 할 거
        TeleportAllStartPoint(startPointActor);

        FTimerHandle timerHandle;
        GetWorld()->GetTimerManager()
        .SetTimer(timerHandle, [this]() mutable
        {
            //타이머에서 할 거
            // 이륙 성공 보이스 재생
            auto allPC = UJ_Utility::GetAllMissionPC(GetWorld());
            for(auto* pc : allPC)
            {
                if(!pc) continue;
                pc->CRPC_PlayCommanderVoice3(static_cast<int>(EMissionProcess::FLIGHT_START));
            }

            // FIXME 팝업 UI 키는 걸로 대체해야함
            // @@ 팝업 UI 끝날때
            // 고정 해제
            for(auto* pc : allPC)
            {
                if(!pc) continue;
                auto* pawn = pc->GetPawn<AL_Viper>();
                if(!pawn) continue;
                // 고정 해제
                pawn->SetEngineOn();
            }
            // 미션 시작
            this->objectiveManagerComp->ActiveNextObjective();
            
        }, 1.5, false);
    }, 3.0f, false);
}
