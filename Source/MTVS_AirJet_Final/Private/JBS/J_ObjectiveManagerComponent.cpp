// Fill out your copyright notice in the Description page of Project Settings.


#include "JBS/J_ObjectiveManagerComponent.h"
#include "Engine/Engine.h"
#include "Engine/EngineTypes.h"
#include "Engine/World.h"
#include "JBS/J_BaseMissionObjective.h"
#include "JBS/J_MissionGamemode.h"
#include "JBS/J_Utility.h"
#include "KHS/K_GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "JBS/J_MissionPlayerController.h"
#include "JBS/J_ObjectiveUIComp.h"
#include "Math/Color.h"
#include "Math/MathFwd.h"
#include "JBS/J_JsonUtility.h"
#include "TimerManager.h"

UJ_ObjectiveManagerComponent::UJ_ObjectiveManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

#pragma region 생성단
/*
BeginPlay
-> 주인 게임모드 캐시
-> 이륙시 (시동, 이륙) 스킵 함수 바인드 | startTODel => SkipDefaultObj

GM 에서 전술명령 생성 및 초기화 요청 | InitDefaultObj
-> 시동 , 이륙 목표 생성| SpawnObjActor
-> 생성한 목표 기본 설정 | objActor->InitObjective
-> 수행도 갱신 함수 바인드 | UpdateObjectiveSuccess
-> 시동 목표는 종료시 이륙 목표 시작 함수 바인드 | objectiveEndDel => StartTakeOffObj
-> 미션 데이터 대로 전술명령들 생성 | InitObjectiveList
-> 보정된 스폰 지점 계산 | GetTransform 
-> 목표 생성 | SpawnObjActor
-> 목표 완료시 다음 목표 활성화 | objectiveEndDel => ActiveNextObjective
-> 목표 완료시 수행도 갱신 함수 바인드 | sendObjSuccessDel => UpdateObjectiveSuccess

호스트의 시작 팝업 종료시 시동 목표 시작 | GM->StartDefaultObjective
-> 시동 목표 시작 | StartDefualtObj
-> 목표 활성화 | DelayedObjectiveActive => obj->IS_OBJECTIVE_ACTIVE = true
-> 시동 목표 종료시 이륙 목표 시작 | StartTakeOffObj
*/
void UJ_ObjectiveManagerComponent::BeginPlay()
{
	Super::BeginPlay();
	
	// 주인 게임모드 설정
	ownerGM = this->GetOwner<AJ_MissionGamemode>();

	// 이륙 시 기본 목표 스킵 바인드
	ownerGM->startTODel.AddDynamic(this, &UJ_ObjectiveManagerComponent::SkipDefaultObj);
}

void UJ_ObjectiveManagerComponent::InitDefaultObj()
{
	if(!GetOwner()->HasAuthority()) return;
	
	for(auto& dmData : defaultObjDataAry)
	{
		// 프리팹 가져오기
		auto* objActor = SpawnObjActor(dmData.objType);

		// 목표 액터 설정
		dmData.objectiveActor = objActor;

		// 목표 수행도 갱신함수 바인드
		objActor->sendObjSuccessDel.AddUObject(this, &UJ_ObjectiveManagerComponent::UpdateObjectiveSuccess);
		
		// 시동 절차 끝나고 이륙 절차 시작
		if(dmData.objType == ETacticalOrder::ENGINE_START)
		{
			objActor->objectiveEndDel.AddDynamic( this, &UJ_ObjectiveManagerComponent::StartTakeOffObj);
		}
	}
}

// 목표 액터 스폰 
AJ_BaseMissionObjective*  UJ_ObjectiveManagerComponent::SpawnObjActor(const ETacticalOrder& type, const FTransform &spawnTR)
{
	// 목표 종류에 따라 프리팹 가져오기
	const auto& prefab = objectiveActorPrefabMap[type];

	// 항상 스폰 처리
	FActorSpawnParameters params;
	params.bNoFail = true;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// 목표 액터 스폰
	auto* objectiveActor = GetWorld()
		->SpawnActor<AJ_BaseMissionObjective>(prefab, spawnTR, params);

	// 목표 액터 설정
	objectiveActor->InitObjective(type, false);

	return objectiveActor;
}

void UJ_ObjectiveManagerComponent::InitObjectiveList(const TArray<FMissionObject>& missions)
{
	// 목표 배열 초기화
	objectiveDataAry.Init(FObjectiveData(), missions.Num());

	// 데이터 가지고 미션 액터 하나씩 생성
	for(const auto& mData : missions)
	{
		// 스폰 지점 계산
		FTransform spawnTR = mData.GetTransform();
		spawnTR.SetRotation(FQuat(FRotator::ZeroRotator));

		// 목표 액터 스폰
		auto* objectiveActor = SpawnObjActor(mData.GetOrderType(), spawnTR);

		// 목표 완료시 다음 목표 활성화 바인드
		objectiveActor->objectiveEndDel.AddDynamic(this, &UJ_ObjectiveManagerComponent::ActiveNextObjective);
		// 목표 수행도 갱신함수 바인드
		objectiveActor->sendObjSuccessDel.AddUObject(this, &UJ_ObjectiveManagerComponent::UpdateObjectiveSuccess);
	
		// 목표 액터 배열에 추가
		// 순서대로 추가하기위해 pinNo 사용
		objectiveDataAry[mData.pinNo].objectiveActor = objectiveActor;
		objectiveDataAry[mData.pinNo].objType = objectiveActor->orderType;
	}

	// UJ_Utility::PrintFullLog(FString::Printf(TEXT("목표 액터 추가 완료 개수 : %d"), objectiveDataAry.Num()), 3.f, FColor::White);
}

// 기본 목표 시작 | 시동
void UJ_ObjectiveManagerComponent::StartDefualtObj()
{
	auto* engineObj = defaultObjDataAry[0].objectiveActor;
	if(!engineObj)
	{
		UJ_Utility::PrintFullLog(TEXT("Invalid DefaultObj Start"), 3.f, FColor::Red);
		return;
	}

	// 활성화
	DelayedObjectiveActive(engineObj, 0.001f);
}

void UJ_ObjectiveManagerComponent::StartTakeOffObj()
{
	// 미션 시작 시 활성화 안함
	if(CUR_ACTIVE_MISSION_IDX >= 0)
	{
		UJ_Utility::PrintFullLog(TEXT("미션 이미 시작됨"), 3.f, FColor::Red);
		return;
	}
	// 이륙 목표
	auto* takeOffObj = defaultObjDataAry[1].objectiveActor;
	// 유효성 체크
	if(ownerGM->isTPReady
	|| !takeOffObj
	|| takeOffObj->IS_OBJ_ENDED)
		return;

	// 활성화
	DelayedObjectiveActive(takeOffObj, objSwitchInterval);
}

#pragma endregion

#pragma region 목표 활성화단
/*
GM 에서 이륙 처리 후 전술명령 시작 | ActiveNextObjective
-> 현재 활성 목표 idx 증가 | SetCurActiveMissionIdx
-> 결산 체크 | true => MissionComplete
-> 해당 인덱스 목표 활성화 | ActiveObjectiveByIdx
*/

void UJ_ObjectiveManagerComponent::ActiveNextObjective()
{
	// 인덱스 증가
	CUR_ACTIVE_MISSION_IDX++;
	// 결산 체크
	if(isMissionComplete) return;

	// 최초가 아니면 종료 애니메이션 대기 (delay 애니메이션에 맞게 수정 필요)
	ActiveObjectiveByIdx(CUR_ACTIVE_MISSION_IDX, CUR_ACTIVE_MISSION_IDX == 0);
}

void UJ_ObjectiveManagerComponent::SetCurActiveMissionIdx(int value)
{
	if(!GetOwner()->HasAuthority()) return;
	
	if(value < 0 || value > objectiveDataAry.Num())
	{
		UJ_Utility::PrintFullLog(TEXT("Invalid Mission Idx"), 3.f, FColor::Red);
		return;
	}
	// 값 적용
    curActiveMissionIdx = value;
	// 결산 체크
    if (CUR_ACTIVE_MISSION_IDX >= objectiveDataAry.Num())
		MissionComplete();
}

void UJ_ObjectiveManagerComponent::ActiveObjectiveByIdx(int mIdx, bool isFirst)
{
	if(mIdx >= objectiveDataAry.Num())
		return;

	// 목표 액터 가져오기
	auto* obj = objectiveDataAry[mIdx].objectiveActor;
	if(!obj) return;
	
	// 딜레이 여부
	float delayTime = isFirst ? 0.001f : objSwitchInterval;

	// 활성화
	DelayedObjectiveActive(obj, delayTime);
}

void UJ_ObjectiveManagerComponent::DelayedObjectiveActive(AJ_BaseMissionObjective *obj, float delayTime)
{
	if(!obj) return;

	FTimerHandle timerHandle2;
	GetWorld()->GetTimerManager()
		.SetTimer(timerHandle2, [this, obj]() mutable
	{
		if(!obj) return;

		obj->IS_OBJECTIVE_ACTIVE = true;
		CUR_ACTIVE_MISSION = obj;		
	}, delayTime, false);
}

#pragma endregion

#pragma region 목표 데이터 처리 단

#pragma endregion

#pragma region 미션 종료(결산) 단
/*
미션 종료 딜리게이트 실행 | missionEndDel
모든 목표 데이터 결합 | fullObjData
AI 피드백 요청 | RequestExecute | EJsonType::AI_FEEDBACK , aiFeedbackResUseDel => ResSendResultData
->
*/
void UJ_ObjectiveManagerComponent::MissionComplete()
{
	isMissionComplete = true;
	CUR_ACTIVE_MISSION = nullptr;
	// 여기부터 결산 단계

	// 미션 종료 딜리게이트 실행
	ownerGM->missionEndDel.Broadcast();
	// 시동,이륙, 목표 데이터를 다 가진 전체 데이터
	TArray<FObjectiveData> fullObjData;
	fullObjData.Append(defaultObjDataAry);
	fullObjData.Append(objectiveDataAry);

	// 결산 데이터 가지고 ai 피드백 요청
	// 게임 인스턴스 가져와서 만들어둔 딜리게이트에 내 함수 바인딩
	auto* gi = UJ_Utility::GetKGameInstance(GetWorld());
	gi->aiFeedbackResUseDel.BindUObject(this, &UJ_ObjectiveManagerComponent::ResSendResultData);

	// 결과 데이터 제작
	TArray<float> spAry;
    Algo::Transform(fullObjData, spAry, [](FObjectiveData temp){
        return temp.successPercent;
    });
    //캐스트 후
    const FAIFeedbackReq feedbackReq(spAry);

	// 서버에 요청 시작 -> 1~4 단계를 거쳐 바인드한 함수에 데이터가 들어옴.
	UJ_JsonUtility::RequestExecute(GetWorld(), EJsonType::AI_FEEDBACK, feedbackReq);

	// 미션 완료 ui 전환
	FTimerHandle timerHandle;
	GetWorld()->GetTimerManager()
		.SetTimer(timerHandle, [this, fullObjData]() mutable
	{
		//타이머에서 할 거
		auto allPC = UJ_Utility::GetAllMissionPC(GetWorld());

		for(AJ_MissionPlayerController* pc : allPC)
		{
			// 결산 UI 전환
			pc->objUIComp->CRPC_SwitchResultUI(fullObjData);
		}
	}, objSwitchInterval, false);
}

void UJ_ObjectiveManagerComponent::ResSendResultData(const FAIFeedbackRes &resData)
{
	// 결과 데이터 결과 ui에 전달
	auto allPC = UJ_Utility::GetAllMissionPC(GetWorld());
	for(auto* pc : allPC)
	{
		pc->objUIComp->CRPC_SetResultAIFeedback(resData);
	}
}

#pragma endregion

#pragma region 기타

#pragma endregion











void UJ_ObjectiveManagerComponent::UpdateObjectiveSuccess(AJ_BaseMissionObjective* objActor, float successPercent)
{
	// 사용 목표 찾기
	FObjectiveData* data = defaultObjDataAry.FindByPredicate([objActor](const FObjectiveData& objData)
	{
		return objData.objectiveActor == objActor;
	});

	if(!data)
	{
		data = objectiveDataAry.FindByPredicate([objActor](const FObjectiveData& objData)
		{
			return objData.objectiveActor == objActor;
		});
	}
	// 내가 기어코 구조체 포인터를 쓰는구나

	// 갱신
	data->successPercent = successPercent;

	// GEngine->AddOnScreenDebugMessage(-1, -1.f, FColor::Green, FString::Printf(TEXT("도당체 : %.2f"), data.successPercent));
}



void UJ_ObjectiveManagerComponent::SkipDefaultObj(bool isSuccess)
{
	for(const FObjectiveData& objData : defaultObjDataAry)
	{
		// 안 끝난 기본 목표 종료 처리
		auto* objActor = objData.objectiveActor;
		if(!objActor || objActor->IS_OBJ_ENDED) continue;

		// 수행도는 0으로
		objActor->SUCCESS_PERCENT = 0.f;
		// 종료
		objActor->ObjectiveEnd(isSuccess);
	}
}


void UJ_ObjectiveManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// 디버그용 현재 활성화된 목표 표시
	if(enablePrintCurActiveMissionActor)
	{
		if(!CUR_ACTIVE_MISSION) return;
		
		// GEngine->AddOnScreenDebugMessage(-1, -1.f, FColor::Purple, FString::Printf(TEXT("현재 활성화된 목표 : %s"), *CUR_ACTIVE_MISSION->GetName()));
	}
}