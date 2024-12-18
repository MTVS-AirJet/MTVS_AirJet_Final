// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include <JBS/J_Utility.h>
#include "J_MissionGamemode.generated.h"

// 이륙 딜리게이트
DECLARE_DYNAMIC_DELEGATE_TwoParams(FTakeOffDel
                                , class AJ_MissionPlayerController*, pc
                                , bool, isSuccess);

// 이륙 종료 딜리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTakeOffEndDel, bool, isSuccess);

// 미션 시작 딜리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FStartTacticalOrderDel, bool, isSuccess);

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

    // 이륙 후 로딩 스크린 딜레이
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
    float loadingDelay = .5f;

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
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Delegate")
    FTakeOffDel onePilotTakeOffDel;

    // 이륙 종료 딜리게이트
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, BlueprintAssignable, Category="Default|Delegate")
    FTakeOffEndDel takeOffEndDel;

    FStartTacticalOrderDel startTODel;

    // 이륙함
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Values")
    bool isTPReady = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, BlueprintCallable, BlueprintAssignable, Category="Default|Delegate")
    FMissionEndDelegate missionEndDel;

protected:
#pragma region 레벨 시작 단
/*
레벨 시작 시 목표 매니저 컴포넌트 캐시 | BeginPlay
-> 미션 시작 | StartMission
-> 미션 데이터 호스트 게임 인스턴스 에서 가져오기 | LoadMissionData
-> 미션 시작 지점 포인트 액터 생성 | InitMissionStartPoint
-> 전술명령 생성 및 초기화 | objMan->InitDefaultObj, InitObjectiveList
-> 플레이어 시작 위치 설정 | SetSpawnPoints // 지휘관 있었을때의 영향으로 역할에 따라 분류되서 가져옴
-> 다른 액터들이 준비되도록 .1초 타이머 이후 세슘 액터 캐시 | CacheCesiumActors
*/
    virtual void BeginPlay() override;

    // 미션 시작
    UFUNCTION(BlueprintCallable)
    virtual void StartMission();

    // 호스트 GI에서 미션 데이터 로드
    FMissionDataRes LoadMissionData();

    // 미션 시작지점 액터 추가 | 첫 미션 바라보게
    void InitMissionStartPoint(const FMissionStartPos &startPointData, const FMissionObject& firstMission = FMissionObject());

    // 레벨에 존재하는 스폰포인트 가져와서 맵에 설정하기
    void SetSpawnPoints(FMissionPlayerSpawnPoints& spawnPointsStruct);

    // 세슘 관련 액터 레벨에서 찾아서 캐시
    void CacheCesiumActors();
#pragma endregion

#pragma region 플레이어 접속 단
/*
플레이어 접속 | PostLogin
-> 호스트 pc 시작 버튼에 미션 시작 함수 바인드 | PostLogin
-> 전투기 생성 요청 | pc->SpawnMyPlayer
-> 게임 인스턴스에서 자기 역할에 맞는 플레이어 프리팹 가져오기 | pc->gi->GetMissionPlayerPrefab
-> 자기 역할에 맞는 스폰위치 가져오기 | GetPlayerSpawnTransfrom // 무조건 파일럿
-> 스폰 포인트 구하기->없으면 월드에서 다시 찾기-> 없으면 생성 | GetSpawnPoint, AddSpawnPoint
-> 플레이어 생성 후 포제스 | SpawnActor, Possess
-> 역할에 따라 마우스 커서 및 입력 처리

미션 시작 버튼 누르면 미션 시작 설정 | StartMissionLevel
-> 시작 보이스 및 자막 재생 | CRPC_PlayCommanderVoice3 | EMissionProcess::MISSION_START
-> 팝업 UI 시작 | CRPC_ActivePopupUI
-> 팝업 UI 종료시 시동 목표 시작 | StartDefaultObjective
-> 미션 시작시 시동 절차 목표 활성화 | mcomp->StartDefualtObj
*/

public:
    // 플레이어 접속
    virtual void PostLogin(APlayerController *newPlayer) override;

    // 해당 역할의 플레이어 스폰 포인트 트랜스폼 가져오기
    FTransform GetPlayerSpawnTransfrom(EPlayerRole role, AJ_MissionPlayerController* pc);

protected:
    // 해당하는 역할의 스폰포인트 가져오기
    class AJ_MissionSpawnPointActor* GetSpawnPoint(EPlayerRole role);

    // 스폰 포인트 액터 추가
    class AJ_MissionSpawnPointActor* AddSpawnPoint(FMissionPlayerSpawnPoints& spawnPointsStruct, const EPlayerRole& addRole);

    // 미션 레벨 시작 트리거 | 호스트가 시작 버튼 누르면 실행
    UFUNCTION(BlueprintCallable)
    void StartMissionLevel();

    // 기본 목표 시작
    UFUNCTION(BlueprintCallable)
    void StartDefaultObjective();

#pragma endregion

#pragma region 이륙 단
/*
텔포 박스에서 충돌시 해당 pc 이륙 체크 | AddFlightedPC
-> 한 명 이륙 딜리게이트 실행 | onePilotTakeOffDel
-> 이륙 목표 액터에서 수행도 갱신 처리 | takeOffObj->SuccessTakeOff
-> 로딩 UI 추가 | pc->CRPC_AddLoadingUI
-> 전체 플레이어 이륙 체크
-> 이륙 종료 딜리게이트 실행 | takeOffEndDel
-> 이륙 목표 종료 처리 | takeOffObj->ObjectiveEnd

전술명령 시작 | StartTacticalOrder
-> 끝내지 않은 기본 목표(시동,이륙) 종료 처리 | startTODel, SkipDefaultObj, ObjectiveEnd
*/
    // 이륙한 pc 배열 추가
public:
    UFUNCTION(BlueprintCallable)
    bool AddFlightedPC(class AJ_MissionPlayerController *pc, bool isSuccess = true);

protected:
    // 전술 명령 미션 시작
    void StartTacticalOrder();

#pragma region 미션 지역 이동 단
/*
약간 딜레이 이후 미션 지역으로 변경 | ChangeMissionArea
-> 미션 지역으로 위경도 변경 | cbox->SetDestinationLogitudeLatitude
-> 미션 지역으로 세슘 머티리얼 변경 | cbox->SRPC_ChangeMissionArea
-> 텔포 박스 콜리전 제거 | boxComp->DestroyComponent
*/
    // 미션 지역으로 세슘 이동
    void ChangeMissionArea();

#pragma endregion
#pragma region 전투기 텔포 단
/*
미션 시작 지점으로 전투기 위치 이동 | TeleportAllStartPoint
-> 각 pc 로딩 ui 제거 | pc->CRPC_RemoveLoadingUI
-> 번호순 파일럿 역할 설정
-> 역할에 따른 위치 계산 | CalcTeleportTransform
-> 계산된 위치로 텔레포트 | pc->MRPC_TeleportStartPoint
*/
    // 미션 시작지점으로 전이
    UFUNCTION(BlueprintCallable)
    void TeleportAllStartPoint(class AJ_MissionStartPointActor *startPoint);

    // 시작 지점 산개하여 배치되기 위해 위치 계산
    FTransform CalcTeleportTransform(const int idx);
    FTransform CalcTeleportTransform(const EPilotRole& role);
#pragma endregion
#pragma region 미션 시작 단
/*
약간 딜레이 이후 미션 시작 | DelayStartTacticalOrder
-> 이륙 보이스 재생 요청 | pc->CRPC_PlayCommanderVoice3, FLIGHT_START
-> 이륙 팝업 UI 활성화 | pc->CRPC_ActivePopupUI
-> 팝업 종료시 첫 번째 전술명령 활성화 | StartFirstTacticalOrder
-> 전투기 고정 해제 | viper->SetEngineOn
-> 첫 목표 시작 | mcomp->ActiveNextObjective
*/
    // 지연된 전술 명령 시작
    void DelayStartTacticalOrder(float delayTime = 0.1f);

    // 첫 번째 전술명령 시작
    UFUNCTION(BlueprintCallable)
    void StartFirstTacticalOrder();

#pragma endregion
#pragma endregion

public:
    virtual void Tick(float DeltaSeconds) override;
};
