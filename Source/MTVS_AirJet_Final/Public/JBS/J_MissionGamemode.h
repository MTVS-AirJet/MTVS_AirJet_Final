// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include <JBS/J_Utility.h>
#include "J_MissionGamemode.generated.h"

/**
 * 
 */
//  로딩 ui 제거
DECLARE_MULTICAST_DELEGATE(FRemoveLoadingUIDel);

// 이륙 딜리게이트
DECLARE_DELEGATE_TwoParams(FTakeOffDel, class AJ_MissionPlayerController*, bool);

// 미션 시작 딜리게이트
DECLARE_MULTICAST_DELEGATE_OneParam(FStartTacticalOrderDel, bool);

// 미션 종료 딜리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMissionEndDelegate);


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

    // 스폰 포인트 정보 구조체
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Objects")
    FMissionPlayerSpawnPoints spawnPoints;

    // 디버그용 더미 미션 데이터 사용
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Debug")
    bool enableUsingDummyMissionData = false;

    // 디버그용 옛날 스폰 시스템 사용
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Debug")
    bool enableUseOldSpawnSystem = false;

    bool isStartTO = false;

    // 시작 지점 액터 프리팹
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Classes")
    TSubclassOf<class AJ_MissionStartPointActor> startPointActorPrefab;
    // 시작 지점 액터
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Objects", BlueprintGetter=GetStartPointActor, BlueprintSetter=SetStartPointActor)
    class AJ_MissionStartPointActor* startPointActor;
        public:
    __declspec(property(get = GetStartPointActor, put = SetStartPointActor)) class AJ_MissionStartPointActor* START_POINT_ACTOR;
    UFUNCTION(BlueprintGetter)
    class AJ_MissionStartPointActor *GetStartPointActor();
    UFUNCTION(BlueprintSetter)
    void SetStartPointActor(class AJ_MissionStartPointActor* value)
    {
        startPointActor = value;
    }
        protected:

    // 편대 진형 데이터
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
    FFormationValue formationData;

    // 플레이어 스폰 횟수 기록 | 산개 로직 사용
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
    int spawnCnt = 0;

    // 스폰 간격 거리
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
    float spawnInterval = 500.f;

#pragma region 세슘 관련 액터
    // 활주로 엔드 포인트 및 위경도 위치 이동 담당
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Objects")
    class AK_CesiumTeleportBox* cesiumTPBox;
        public:
    __declspec(property(get = GetTPBox, put = SetTPBox)) class AK_CesiumTeleportBox* TP_BOX;
    class AK_CesiumTeleportBox* GetTPBox()
    {
        return cesiumTPBox;
    }
    void SetTPBox(class AK_CesiumTeleportBox* value)
    {
        cesiumTPBox = value;
    }
        protected:

    // 미션의 정점 (0,0,0) 을 나타내는 액터 | 실제 0,0,0은 세슘으로 인해 내핵이 됨.
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Objects")
    class AK_CesiumManager* cesiumZeroPos;
    
#pragma endregion

    // 이륙한 파일럿 pc 배열
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Objects")
    TSet<class AJ_MissionPlayerController*> flightedPCAry;
    
    // 미션 데이터
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
    FMissionDataRes curMissionData;
        public:
    __declspec(property(get = GetCurMissionData, put = SetCurMissionData)) FMissionDataRes CUR_MISSION_DATA;
    FMissionDataRes GetCurMissionData()
    {
        return curMissionData;
    }
    void SetCurMissionData(FMissionDataRes value)
    {
        curMissionData = value;
    }
        protected:

    // 이륙 후 미션 시작 딜레이
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
    float missionStartDelay = 1.5f;
public:
    // XXX 로드할 미션 맵 이름
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
    FString curMissionName;

    // 디버그용 더미 미션 데이터
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Debug")
    FMissionDataRes dummyMissionData;

    int pIdx = 0;

    // 해당 파일럿 이륙 딜리게이트
    FTakeOffDel onePilotTakeOffDel;

    FStartTacticalOrderDel startTODel;

    // 이륙함
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Values")
    bool isTPReady = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Values")
    FMissionEndDelegate missionEndDel;

protected:
    virtual void BeginPlay() override;

    // bp에서 begin에 호출되는 미션 시작
    UFUNCTION(BlueprintCallable)
    virtual void StartMission();

    // 호스트 GI에서 미션 데이터 로드
    FMissionDataRes LoadMissionData();

    // 미션 지역으로 세슘 이동
    void ChangeMissionArea();

    // 첫 번째 전술명령 시작
    UFUNCTION(BlueprintCallable)
    void StartFirstTacticalOrder();

    // 기본 목표 시작
    UFUNCTION(BlueprintCallable)
    void StartDefaultObjective();

    // 해당하는 역할의 스폰포인트 가져오기
    class AJ_MissionSpawnPointActor* GetSpawnPoint(EPlayerRole role);

    // 레벨에 존재하는 스폰포인트 가져와서 맵에 설정하기
    void SetSpawnPoints(FMissionPlayerSpawnPoints& spawnPointsStruct);

    // 스폰 포인트 액터 추가
    class AJ_MissionSpawnPointActor* AddSpawnPoint(FMissionPlayerSpawnPoints& spawnPointsStruct, EPlayerRole addRole);

    // 0. 미션 레벨 시작 트리거 | 호스트가 시작 버튼 누르면 실행
    UFUNCTION(BlueprintCallable)
    void StartMissionLevel();

    // 1. 미션 시작지점 액터 추가 | 첫 미션 바라보게
    void InitMissionStartPoint(const FMissionStartPos &startPointData, const FMissionObject& firstMission = FMissionObject());

    // 2. 미션 시작지점으로 전이
    UFUNCTION(BlueprintCallable)
    void TeleportAllStartPoint(class AJ_MissionStartPointActor *startPoint);

    // 2-1. 시작 지점 산개하여 배치되기 위해 위치 계산
    FTransform CalcTeleportTransform(int idx);

    FTransform CalcTeleportTransform(EPilotRole role);

    // 세슘 관련 액터 레벨에서 찾아서 캐시
    void CacheCesiumActors();

    // 해당 pc에 로딩 스크린 UI 추가
    UFUNCTION(Server, Reliable)
    void SRPC_RemoveLoadingUIByPC(class AJ_MissionPlayerController *missionPC);

    // 3. 전술 명령 미션 시작
    void StartTacticalOrder();

    // 지연된 전술 명령 시작
    void DelayStartTacticalOrder(float delayTime = 0.1f);

public:
    virtual void Tick(float DeltaSeconds) override;

    // 플레이어 접속
    virtual void PostLogin(APlayerController *newPlayer) override;

    // 해당 역할의 플레이어 스폰 포인트 트랜스폼 가져오기
    FTransform GetPlayerSpawnTransfrom(EPlayerRole role, AJ_MissionPlayerController* pc);

    // 이륙한 pc 배열 추가
    UFUNCTION(BlueprintCallable)
    bool AddFlightedPC(class AJ_MissionPlayerController *pc, bool isSuccess = true);
};
