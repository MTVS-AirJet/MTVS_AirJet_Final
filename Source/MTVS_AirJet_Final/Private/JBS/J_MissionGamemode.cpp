// Fill out your copyright notice in the Description page of Project Settings.


#include "JBS/J_MissionGamemode.h"
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
#include "Math/MathFwd.h"
#include "JBS/J_ObjectiveManagerComponent.h"
#include "Templates/Casts.h"
#include "TimerManager.h"
#include <JBS/J_MissionSpawnPointActor.h>
#include <Engine/World.h>



void AJ_MissionGamemode::BeginPlay()
{
    Super::BeginPlay();

    // FIXME 호스트 시작 버튼에 딜리게이트 바인드 해야함
    // 임시로 키고 1.5 뒤에 되도록
    FTimerHandle timerHandle;
    GetWorld()->GetTimerManager()
        .SetTimer(timerHandle, [this]() mutable
    {
        //타이머에서 할 거
        StartMissionLevel();
    }, 1.5, false);
}

// 사실상 beginplay
void AJ_MissionGamemode::StartMission()
{
    if(enableUsingDummyMissionData)
    {
        curMissionData = dummyMissionData;
    }
    else {
        curMissionData = LoadMissionData();
    }

    // 시작 지점 액터 생성
    InitMissionStartPoint(curMissionData.startPoint);

    // 목표 매니저 컴포넌트 설정
    check(objectiveManagerComp);
    // 시동 및 이륙 목표 추가
    objectiveManagerComp->InitDefaultObj();
    // 전술 명령 목표 추가
    objectiveManagerComp->InitObjectiveList(curMissionData.mission);

    // XXX 미션 맵 로드
    // LoadMissionMap();

    // 스폰포인트 가져와서 설정
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

void AJ_MissionGamemode::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    
}

void AJ_MissionGamemode::LoadMissionMap()
{
    // 미션 지역 로드
    UGameplayStatics::LoadStreamLevelBySoftObjectPtr(GetWorld(), missionMapPrefab, true, true, FLatentActionInfo());
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

void AJ_MissionGamemode::SetSpawnPoints(FMissionPlayerSpawnPoints& spawnPointsStruct)
{
    // 스폰 포인트 찾기
    TArray<AActor*> outActors;
    
    UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), AJ_MissionSpawnPointActor::StaticClass(), spawnPointsStruct.spawnPointTag, outActors);

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

AJ_MissionSpawnPointActor* AJ_MissionGamemode::AddSpawnPoint(FMissionPlayerSpawnPoints& spawnPointsStruct, EPlayerRole addRole)
{
    // 해당 역할을 가진 스폰 포인트 생성
    auto* spPoint = GetWorld()->SpawnActor<AJ_MissionSpawnPointActor>(spawnPointsStruct.spawnPointPrefab, spawnPointsStruct.spawnPointDefaultPos[addRole]);

    spPoint->spawnType = addRole;

    // 맵에 추가
    spawnPointsStruct.spawnPointMap[addRole] = spPoint;

    return spPoint;
}

// XXX 이제 안쓰니깐 산개 로직 미적용
// 임시 산개 로직 적용
FTransform AJ_MissionGamemode::GetPlayerSpawnTransfrom(EPlayerRole role, AJ_MissionPlayerController* pc)
{
    auto tr = this->GetSpawnPoint(role)->GetActorTransform();
    // 몇 번째 pc 인지 받아서 시작 위치에서 뒤로 - * i 만큼 계산
    // 시작 지점의 뒤 벡터 구하기
    FVector backVector = -1 * tr.GetUnitAxis(EAxis::X);
    backVector *= spawnInterval * tempSpawnCnt;

    // 위치 결정
    tr.SetLocation(tr.GetLocation() + backVector);

    tempSpawnCnt++;

    return tr;
}

void AJ_MissionGamemode::PostLogin(APlayerController *newPlayer)
{
    Super::PostLogin(newPlayer);

    // GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString::Printf(TEXT("플레이어 id : %d"), pIdx));
    // solved 임시로 첫 플레이어는 지휘관 이후는 파일럿으로 역할 설정 | 사용 끝
    // auto* gi = newPlayer->GetGameInstance<UJ_GameInstance>();
    // gi->SetPlayerRole(pIdx == 0 ? EPlayerRole::COMMANDER : EPlayerRole::PILOT);
    
    if(enableUseOldSpawnSystem)
    {
        // 역할 설정후 플레이어 스폰
        auto* pc = CastChecked<AJ_MissionPlayerController>(newPlayer);
        pc->SpawnMyPlayer();
    }

    // pIdx++;
}

void AJ_MissionGamemode::InitMissionStartPoint(const FMissionStartPos &startPointData)
{
    // 시작 지점 위치
    FTransform spawnTR = startPointData.GetTransform();

    // 액터 생성
    auto* spActor = GetWorld()->SpawnActor<AJ_MissionStartPointActor>(startPointActorPrefab, spawnTR);
    check(spActor);
    
    // 캐시
    START_POINT_ACTOR = spActor;
}

void AJ_MissionGamemode::TeleportAllStartPoint(AJ_MissionStartPointActor *startPoint)
{
    if(!HasAuthority()) return;

    // 모든 플레이어의 폰 가져오기
    auto allPawns = UJ_Utility::GetAllMissionPawn(GetWorld());

    // GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString::Printf(TEXT("폰 개수 : %d"), allPawns.Num()));

    // 로딩 ui 제거
    // auto* gs = UJ_Utility::GetMissionGameState(GetWorld());
    // gs->RemoveAllLoadingUI();
    removeLUIDel.Broadcast();


    // 시작 지점으로 위치 이동
    for(int i = 0; i < allPawns.Num(); i++)
    {
        auto* pawn = allPawns[i];
        // 역할 설정
        auto* pc = pawn->GetController<AJ_MissionPlayerController>();
        // FIXME 로딩 ui 할 꺼면 추가 // pc 로딩 ui 잇으면 제거
        pc->SRPC_RemoveLoadingUI();
        // idx 0,1,2 를 각각 리더, lt 윙맨, rt 윙맨으로 설정
        pc->pilotRole = static_cast<EPilotRole>(i % 3);

        // 순서에 따른 위치 조정 (산개용)
        FTransform newTR = CalcTeleportTransform(pc->pilotRole);
        // 스케일 조정되지 않도록 변경
        newTR = FTransform(newTR.GetRotation(), newTR.GetLocation(), pawn->GetActorScale());
        pc->MRPC_TeleportStartPoint(newTR);

        // UE_LOG(LogTemp, Warning, TEXT("type : tr계산단, 나는 누구인가 : %s, tr : %s"), *pc->GetName(), *newTR.ToString());
        // UE_LOG(LogTemp, Warning, TEXT("type : pawn적용단, 나는 누구인가 : %s, tr : %s"), *pc->GetName(), *pawn->GetActorTransform().ToString());
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

void AJ_MissionGamemode::CacheCesiumActors()
{
    // 텔포 박스 가져오기
    auto* tempTPBox = Cast<AK_CesiumTeleportBox>(
        UGameplayStatics::GetActorOfClass(GetWorld(), AK_CesiumTeleportBox::StaticClass()));
    check(tempTPBox);
    cesiumTPBox = tempTPBox;

    // 정점 액터 가져오기
    auto* tempZeroPos = Cast<AK_CesiumManager>(
        UGameplayStatics::GetActorOfClass(GetWorld(), AK_CesiumManager::StaticClass()));
    check(tempZeroPos);
    cesiumZeroPos = tempZeroPos;

    // GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("무사히 세슘 관련 액터 캐시 성공"));
}

// 이륙 체크 및 미션 시작 단
bool AJ_MissionGamemode::AddFlightedPC(class AJ_MissionPlayerController *pc)
{
    // GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("이륙 추가 : %s"), *pc->GetName()));
    // 이륙 배열에 추가
    flightedPCAry.Add(pc);

    // 해당 pc에게 로딩 UI 추가
    pc->CRPC_AddLoadingUI();

    // 배열 크기가 플레이어 수와 같아지면 시작 지점 텔포 및 미션 시작
    bool isTPReady = flightedPCAry.Num() == GetGameState<AJ_MissionGameState>()->GetAllPlayerController().Num();
    if(isTPReady)
        StartTacticalOrder();

    return isTPReady;
}



void AJ_MissionGamemode::SRPC_RemoveLoadingUIByPC_Implementation(class AJ_MissionPlayerController *missionPC)
{
    
}

FMissionDataRes AJ_MissionGamemode::LoadMissionData()
{
    auto* gi = UJ_Utility::GetKGameInstance(GetWorld());
    
    return gi->MissionData;
}

void AJ_MissionGamemode::StartMissionLevel()
{
    // 시동 목표 시작
    objectiveManagerComp->StartDefualtObj();
}

void AJ_MissionGamemode::StartTacticalOrder()
{
    // @@ 기본 목표 종료 처리
    // GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::White, TEXT("전부 이륙"));

    // @@ 임시로 조금 늦게 텔포
    FTimerHandle timerHandle;
    GetWorld()->GetTimerManager()
        .SetTimer(timerHandle, [this]() mutable
    {
        //타이머에서 할 거
        TeleportAllStartPoint(startPointActor);

        FTimerHandle timerHandle;
        GetWorld()->GetTimerManager()
        .SetTimer(timerHandle, [this]() mutable
        {
            //타이머에서 할 거
            // 미션 시작
            // @@ 임시로 시작 늦게 | 시작 anim 보여주고 싶음 | 나중엔 로딩 뽕맛 보여줘야지
            this->objectiveManagerComp->ActiveNextObjective();
            
        }, 1.5, false);
    }, 1.5f, false);
}
