// Fill out your copyright notice in the Description page of Project Settings.


#include "JBS/J_MissionGamemode.h"
#include "Engine/LatentActionManager.h"
#include "JBS/J_Utility.h"
#include "Kismet/GameplayStatics.h"
#include "Math/MathFwd.h"
#include <JBS/J_MissionSpawnPointActor.h>
#include <Engine/World.h>



void AJ_MissionGamemode::BeginPlay()
{
    Super::BeginPlay();

    // FIXME 미션 데이터 불러올 수 있게되면 아래 함수들 통합 필요
    // LoadMissionData();

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
    auto tr = this->GetSpawnPoint(role)->GetActorTransform();;

    return tr;
}