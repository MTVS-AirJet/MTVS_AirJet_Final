// Fill out your copyright notice in the Description page of Project Settings.


#include "JBS/J_ObjectiveUIComp.h"
#include "Blueprint/UserWidget.h"
#include "Components/SlateWrapperTypes.h"
#include "Containers/UnrealString.h"
#include "Engine/Engine.h"
#include "JBS/J_MissionPlayerController.h"
#include "JBS/J_ObjectiveUI.h"
#include "JBS/J_Utility.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/Class.h"
#include "JBS/J_ObjectiveTextUI.h"

// Sets default values for this component's properties
UJ_ObjectiveUIComp::UJ_ObjectiveUIComp()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UJ_ObjectiveUIComp::BeginPlay()
{
	Super::BeginPlay();
	
	auto* myPC = GetOwner<AJ_MissionPlayerController>();
	
	// 클라에서 및 호스트 클라에서 처리
	if(!(myPC->HasAuthority()) || myPC->HasAuthority() && myPC->IsLocalController())
	{
		// GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::White, TEXT("목표 UI 생성2"));
		// 목표 UI 생성
		InitObjUI();
	}
}

// Called every frame
void UJ_ObjectiveUIComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UJ_ObjectiveUIComp::InitObjUI()
{
	// GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::White, TEXT("목표 UI 생성"));
	// ui 생성
	objUI = CreateWidget<UJ_ObjectiveUI>(this->GetOwner<AJ_MissionPlayerController>(), objUIPrefab);
	check(objUI);
	// @@ 애니메이션 바인드
	// objUI->startAnimDel.AddUObject(this, &UJ_ObjectiveUIComp::PlayObjUIStartAnim);

	// 비활성화
	objUI->SetVisibility(ESlateVisibility::Hidden);
	// 뷰포트에 붙이기
	objUI->AddToViewport();
}

void UJ_ObjectiveUIComp::CRPC_StartObjUI_Implementation(FTacticalOrderData orderData)
{
	// 비활성 상태시 활성화
	if(objUI->GetVisibility() == ESlateVisibility::Hidden)
		objUI->SetVisibility(ESlateVisibility::Visible);

	// 시작 처리
	objUI->StartObjUI();
	// 값 설정
	CRPC_UpdateObjUI(orderData, true);
}

void UJ_ObjectiveUIComp::CRPC_UpdateObjUI_Implementation(FTacticalOrderData orderData, bool isInit)
{
	TArray<FTextUIData> textUIData;

	// 전술명령 종류에 따라 다른 구조체 사용해 ui 값 생성
	switch (orderData.orderType) {
        case ETacticalOrder::NONE:
        case ETacticalOrder::MOVE_THIS_POINT:
			break;
        case ETacticalOrder::FORMATION_FLIGHT:
		{
			auto& flightData = orderData.ffData;

			CreateUIData(flightData, textUIData, isInit);

		}
			break;
        case ETacticalOrder::NEUTRALIZE_TARGET:
		{
			auto& ntData = orderData.ntData;

			CreateUIData(ntData, textUIData, isInit);
		}
            break;
		case ETacticalOrder::ENGINE_START:
		{
			auto& epData = orderData.epData;

			CreateUIData(epData, textUIData, isInit);
		}
			break;

	}
	// GEngine->AddOnScreenDebugMessage(-1, -1.f, FColor::White, TEXT("doremi111"));

	// ui에 값 전달
	if(isInit)
		objUI->SetObjUI(textUIData, isInit);
	else
	 	objUI->SetObjUI(textUIData[0], isInit);
}

void UJ_ObjectiveUIComp::CRPC_EndSubObjUI_Implementation(int idx, bool isSuccess)
{
	objUI->EndSubObjUI(idx, isSuccess);
}

void UJ_ObjectiveUIComp::CRPC_EndObjUI_Implementation(bool isSuccess)
{
	objUI->EndObjUI(isSuccess);
}

UJ_MissionCompleteUI *UJ_ObjectiveUIComp::GetMissionCompleteUI()
{
	check(objUI);
	
	return objUI->missionCompleteUI;
}

void UJ_ObjectiveUIComp::CRPC_SwitchResultUI_Implementation(const TArray<FObjectiveData>& resultObjData)
{
	objUI->ActiveResultUI(resultObjData);
}
// @@ 이 시점에서 텍스트 스타일 결정해야함
void UJ_ObjectiveUIComp::CreateUIData(const FFormationFlightUIData &data, TArray<FTextUIData>& outData, bool isInit)
{
	// 목표 ui 데이터
	FTextUIData objUIData;
	// 상세 ui 데이터
	FTextUIData detailUIData;

	// 목표 단
	FString checkHeightStr = FString::FormatAsNumber(data.checkHeight * 3.281 / 100);
	FString curHeightStr = FString::FormatAsNumber(data.curHeight * 3.281 / 100);

	objUIData.headerText = FRichString(TEXT("편대 비행"));
	objUIData.bodyTextAry = {
		FRichString(FString::Printf(TEXT("편대 비행 중 : %s"), *UJ_Utility::ToStringBool(data.checkFormation)))
		,FRichString(FString::Printf(TEXT("목표 고도 %s ft: \n현재 %s ft"), *checkHeightStr, *curHeightStr))
		,FRichString(FString::Printf(TEXT("올바른 위치 : %s"), *UJ_Utility::ToStringBool(data.isCorrectPosition)))
		,FRichString(FString::Printf(TEXT("당신의 역할 : %s"), *UJ_Utility::PilotRoleToString(data.pilotRole)))
	};

	if(isInit)
	{
		// 상세 단
		detailUIData.headerText = FRichString(TEXT("임시 편대 상세 텍스트"));
		detailUIData.bodyTextAry = {
			FRichString(TEXT("임시 상세 1"))
			,FRichString(TEXT("doremi 상세 2"))
			,FRichString(TEXT("임시 상세 3"))
		};
	}


	outData = TArray<FTextUIData> { objUIData , detailUIData};
}

void UJ_ObjectiveUIComp::CreateUIData(const FNeutralizeTargetUIData &data, TArray<FTextUIData> &outData, bool isInit)
{
	// 목표 ui 데이터
	FTextUIData objUIData;
	// 상세 ui 데이터
	FTextUIData detailUIData;

	// 목표 단
	objUIData.headerText = FRichString(TEXT("지상 목표 무력화"));
	objUIData.bodyTextAry.Add(FRichString(FString::Printf(TEXT("남은 지상 목표 %d/%d"), data.curTargetAmt, data.allTargetAmt)));

	// 상세 단
	if(isInit)
	{
		detailUIData.headerText = FRichString(TEXT("임시 지대공 상세 텍스트"));
		detailUIData.bodyTextAry = {
			FRichString(TEXT("임시 상세 1"))
			,FRichString(TEXT("doremi 상세 2"))
		};
	}

	outData = TArray<FTextUIData> { objUIData , detailUIData};
}

void UJ_ObjectiveUIComp::CreateUIData(const FEngineProgressData &data, TArray<FTextUIData> &outData, bool isInit )
{
	// 목표 ui 데이터
	FTextUIData objUIData;
	// 상세 ui 데이터
	FTextUIData detailUIData;

	// 목표 단
	objUIData.headerText = FRichString(TEXT("시동 절차"));
	// 시동 절차 텍스트 설정
	for(int i = static_cast<int>(EEngineProgress::MIC_SWITCH_ON); i <= static_cast<int>(EEngineProgress::RELEASE_SIDE_BREAK); i ++)
	{
		// 수행 절차
		EEngineProgress type = static_cast<EEngineProgress>(i);
		ETextStyle style = ETextStyle::DEFAULT;
		// 수행된 절차 확인
		if(type < data.curProgress)
		{
			// 성공 여부
			bool isSuccess = data.CheckProgressSuccess(type);
			// 텍스트 스타일
			style = isSuccess ? ETextStyle::SUCCESS : ETextStyle::FAIL;
		}
		// 리치 텍스트 설정
		FRichString str(data.ToStringProgressEnum(type), style);
		// 본문에 추가
		objUIData.bodyTextAry.Add(str);

		// FIXME 설명을 objdetail 태그로 \n 해서 추가
	}

	// 상세 단
	if(isInit)
	{
		detailUIData.headerText = FRichString(TEXT("임시 시동 절차 상세 텍스트"));
		detailUIData.bodyTextAry = {
			FRichString(FString::Printf(TEXT("임시 텍스트 %d"), FMath::RandRange(1, 10)))
			,FRichString(FString::Printf(TEXT("임시 텍스트 %d"), FMath::RandRange(1, 10)))
		};
	}

	outData = TArray<FTextUIData> { objUIData , detailUIData};
}

