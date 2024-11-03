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
	objUI->startAnimDel.AddUObject(this, &UJ_ObjectiveUIComp::PlayObjUIStartAnim);

	// 비활성화
	objUI->SetVisibility(ESlateVisibility::Hidden);
	// 뷰포트에 붙이기
	objUI->AddToViewport();
}

void UJ_ObjectiveUIComp::CRPC_StartFFObjUI_Implementation(ETacticalOrder orderType, FFormationFlightUIData data)
{
	// 비활성 상태시 활성화
	if(objUI->GetVisibility() == ESlateVisibility::Hidden)
		objUI->SetVisibility(ESlateVisibility::Visible);

	// 시작 처리
	objUI->StartObjUI();

	// UE_LOG(LogTemp, Warning, TEXT("22 objui pc 이름 : %s, 역할 : %s"), *this->GetOwner()->GetName(), *UJ_Utility::PilotRoleToString(data.pilotRole));
	// 값 설정
	CRPC_UpdateFFObjUI(orderType, data);
}

void UJ_ObjectiveUIComp::CRPC_StartObjUI_Implementation(ETacticalOrder orderType, FNeutralizeTargetUIData data)
{
	// 비활성 상태시 활성화
	if(objUI->GetVisibility() == ESlateVisibility::Hidden)
		objUI->SetVisibility(ESlateVisibility::Visible);

	// 시작 처리
	objUI->StartObjUI();
	// 값 설정
	CRPC_UpdateObjUI(orderType, data);
}

void UJ_ObjectiveUIComp::CRPC_UpdateFFObjUI_Implementation(ETacticalOrder orderType, FFormationFlightUIData data)
{
	//@@ 나중엔 오더 타입에 따라 다른 데이터 구조체 처리
	FTextUIData uiData;

	switch (orderType) {
        case ETacticalOrder::NONE:
        case ETacticalOrder::MOVE_THIS_POINT:
			break;
        case ETacticalOrder::FORMATION_FLIGHT:
		{
			FString checkHeightStr = FString::FormatAsNumber(data.checkHeight * 3.281 / 100);
			FString curHeightStr = FString::FormatAsNumber(data.curHeight * 3.281 / 100);
			uiData.headerText = TEXT("편대 비행");
			uiData.bodyTextAry.Add(FString::Printf(TEXT("편대 비행 중 : %s"), data.checkFormation ? TEXT("TRUE") : TEXT("FALSE")));
			uiData.bodyTextAry.Add(FString::Printf(TEXT("목표 고도 %s ft: \n현재 %s ft"), *checkHeightStr, *curHeightStr));
			uiData.bodyTextAry.Add(FString::Printf(TEXT("올바른 위치 : %s"), data.isCorrectPosition ? TEXT("TRUE") : TEXT("FALSE")));
			uiData.bodyTextAry.Add(FString::Printf(TEXT("당신의 역할 : %s")
				, *UJ_Utility::PilotRoleToString(data.pilotRole)));
		}
			break;
        // case ETacticalOrder::NEUTRALIZE_TARGET:
		// {
		// 	auto ntData = static_cast<FNeutralizeTargetUIData>(data);
			
		// 	uiData.headerText = TEXT("지상 목표 무력화");
		// 	uiData.bodyTextAry.Add(FString::Printf(TEXT("남은 지상 목표 %d/%d"), ntData.curTargetAmt, ntData.allTargetAmt));
		// }
            break;
	}

	// UE_LOG(LogTemp, Warning, TEXT("pc 이름 : %s, 역할 : %s"), *this->GetOwner()->GetName(), *UJ_Utility::PilotRoleToString(data.pilotRole));

	// ui에 값 전달
	objUI->SetObjUI(uiData);
}

void UJ_ObjectiveUIComp::CRPC_UpdateObjUI_Implementation(ETacticalOrder orderType, FNeutralizeTargetUIData data)
{
	//@@ 나중엔 오더 타입에 따라 다른 데이터 구조체 처리
	FTextUIData uiData;

	switch (orderType) {
        case ETacticalOrder::NONE:
        case ETacticalOrder::MOVE_THIS_POINT:
			break;
        case ETacticalOrder::FORMATION_FLIGHT:
			break;
        case ETacticalOrder::NEUTRALIZE_TARGET:
		{
			auto ntData = static_cast<FNeutralizeTargetUIData>(data);
			
			uiData.headerText = TEXT("지상 목표 무력화");
			uiData.bodyTextAry.Add(FString::Printf(TEXT("남은 지상 목표 %d/%d"), ntData.curTargetAmt, ntData.allTargetAmt));
		}
            break;
	}

	// ui에 값 전달
	objUI->SetObjUI(uiData);
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
void UJ_ObjectiveUIComp::UpdateObjUIAnimValue(float canvasX, float bgPaddingBottom, float subEleScaleY)
{
	// FString str = FString::Printf(TEXT("cX : %.2f , bpb : %.2f, sesy : %.2f"), canvasX, bgPaddingBottom, subEleScaleY);
	// GEngine->AddOnScreenDebugMessage(-1, -1.f, FColor::Purple, FString::Printf(TEXT("%s"), *str));

	// ui 요소에 적용
	objUI->GetObjTextUI()->UpdateObjUIAnimValue(canvasX, bgPaddingBottom, subEleScaleY);
}
