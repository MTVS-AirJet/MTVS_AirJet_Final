// Fill out your copyright notice in the Description page of Project Settings.


#include "JBS/J_MissionGamemode.h"
#include "Engine/Engine.h"
#include "Engine/LatentActionManager.h"
#include "JBS/J_GameInstance.h"
#include "JBS/J_MissionGameState.h"
#include "JBS/J_MissionPlayerController.h"
#include "JBS/J_MissionStartPointActor.h"
#include "JBS/J_Utility.h"
#include "Kismet/GameplayStatics.h"
#include "Math/MathFwd.h"
#include "JBS/J_ObjectiveManagerComponent.h"
#include "Templates/Casts.h"
#include <JBS/J_MissionSpawnPointActor.h>
#include <Engine/World.h>



void AJ_MissionGamemode::BeginPlay()
{
    Super::BeginPlay();

    
}

// 사실상 beginplay
void AJ_MissionGamemode::StartMission()
{
    // FIXME 미션 데이터 불러올 수 있게되면 아래 함수들 통합 필요

    if(enableUsingDummyMissionData)
    {
        curMissionData = dummyMissionData;
    }
    else {
        // LoadMissionData();
    }

    // 시작 지점 액터 생성
    InitMissionStartPoint(curMissionData.startPoint);

    // 목표 매니저 컴포넌트 설정
    check(objectiveManagerComp);
    objectiveManagerComp->InitObjectiveList(curMissionData.mission);

    // @@ 0번 목표 시작
    objectiveManagerComp->ActiveNextObjective();
    
    

    // 미션 맵 로드
    LoadMissionMap();

    // 스폰포인트 가져와서 설정
    SetSpawnPoints(spawnPoints);
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

FTransform AJ_MissionGamemode::GetPlayerSpawnTransfrom(EPlayerRole role)
{
    // FIXME 나중에 여러 파일럿일때 적절히 산개시키는 로직 추가해야함
    auto tr = this->GetSpawnPoint(role)->GetActorTransform();

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
    auto* gs = this->GetGameState<AJ_MissionGameState>();
    auto allPawns = gs->GetAllPlayerPawn();

    // GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString::Printf(TEXT("폰 개수 : %d"), allPawns.Num()));
    // FIXME 멀티일때 pawn이 로컬 빼고 null인 오류 수정 필요
    // 시작 지점으로 위치 이동
    for(int i = 0; i < allPawns.Num(); i++)
    {
        auto pawn = allPawns[i];
        // 순서에 따른 위치 조정 (산개용)
        FTransform newTR = CalcTeleportTransform(i);
        // 스케일 조정되지 않도록 변경
        newTR = FTransform(newTR.GetRotation(), newTR.GetLocation(), pawn->GetActorScale());

        pawn->SetActorTransform(newTR);
    }
}

AJ_MissionStartPointActor *AJ_MissionGamemode::GetStartPointActor()
{
    // 없으면 그냥 맵 중앙으로 설정
    if(startPointActor == nullptr)
    {
        GEngine->AddOnScreenDebugMessage(-1, 13.f, FColor::Red, TEXT("미션 시작 지점 미설정 | 기본 위치 적용"));

        FMissionStartPos& dummySPData = dummyMissionData.startPoint;
        InitMissionStartPoint(dummySPData);
    }


    return startPointActor;
}

FTransform AJ_MissionGamemode::CalcTeleportTransform(int idx)
{
    return CalcTeleportTransform(static_cast<EPilotRole>(idx));
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

    baseTR.SetLocation(baseTR.GetLocation() + addVec);

    return baseTR;
}
