// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include <JBS/J_Utility.h>
#include "J_MissionGamemode.generated.h"

/**
 * 
 */
UCLASS()
class MTVS_AIRJET_FINAL_API AJ_MissionGamemode : public AGameModeBase
{
	GENERATED_BODY()

protected:
    // 목표 매니저 컴포넌트
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Component")
    class UJ_ObjectiveManagerComponent* objectiveManagerComp;

    // 미션 지역 맵
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Classes")
	TSoftObjectPtr<UWorld> missionMapPrefab;

    // // 지통실 맵
    // UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Classes")
    // TSoftObjectPtr<UWorld> commandMapPrefab;

    // 스폰 포인트 정보 구조체
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Objects")
    FMissionPlayerSpawnPoints spawnPoints;

    // 디버그용 더미 미션 데이터 사용
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Debug")
    bool enableUsingDummyMissionData = false;

public:
    // 로드할 미션 맵 이름
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
    FString curMissionName;
    // 미션 데이터
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
    FMissionDataRes curMissionData;

    // 디버그용 더미 미션 데이터
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Debug")
    FMissionDataRes dummyMissionData;

protected:
    virtual void BeginPlay() override;

    // bp에서 begin에 호출되는 미션 시작
    UFUNCTION(BlueprintCallable)
    virtual void StartMission();

    void LoadMissionMap();

    // 해당하는 역할의 스폰포인트 가져오기
    class AJ_MissionSpawnPointActor* GetSpawnPoint(EPlayerRole role);

    // 레벨에 존재하는 스폰포인트 가져와서 맵에 설정하기
    void SetSpawnPoints(FMissionPlayerSpawnPoints& spawnPointsStruct);

    // 스폰 포인트 액터 추가
    class AJ_MissionSpawnPointActor* AddSpawnPoint(FMissionPlayerSpawnPoints& spawnPointsStruct, EPlayerRole addRole);

    int pIdx = 0;

public:
    virtual void Tick(float DeltaSeconds) override;

    // 플레이어 접속
    virtual void PostLogin(APlayerController *newPlayer) override;

    // 해당 역할의 플레이어 스폰 포인트 트랜스폼 가져오기
    FTransform GetPlayerSpawnTransfrom(EPlayerRole role);

   
};
