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
#include "JBS/J_MissionCompleteUI.h"


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

#pragma region 목표 ui 초기화 단
	// 주인 pc
	auto* ownerPC = this->GetOwner<AJ_MissionPlayerController>();

	// 로비 돌아가기 함수 바인드
	this->GetMissionCompleteUI()->returnLobbyDel.BindUObject(ownerPC, &AJ_MissionPlayerController::TravelToLobbyLevel);

#pragma endregion

	// 비활성화
	objUI->SetVisibility(ESlateVisibility::Hidden);
	// 뷰포트에 붙이기
	objUI->AddToViewport();
}

void UJ_ObjectiveUIComp::CRPC_StartObjUI_Implementation(const FTacticalOrderData& orderData)
{
	// 비활성 상태시 활성화
	if(objUI->GetVisibility() == ESlateVisibility::Hidden)
		objUI->SetVisibility(ESlateVisibility::Visible);

	// 시작 처리
	objUI->StartObjUI();
	// 값 설정
	CRPC_UpdateObjUI(orderData, true);
}

void UJ_ObjectiveUIComp::CRPC_UpdateObjUI_Implementation(const FTacticalOrderData& orderData, bool isInit)
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
		case ETacticalOrder::TAKE_OFF:
		{
			auto& toData = orderData.toData;

			CreateUIData(toData, textUIData, isInit);
		}
			break;

	}

	// ui 데이터 확인
	if(!(textUIData.Num() > 0))
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("obuicomp : ui 데이터 없음"));

		return;
	}

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
// 이 시점에서 텍스트 스타일 결정해야함
void UJ_ObjectiveUIComp::CreateUIData(const FFormationFlightUIData &data, TArray<FTextUIData>& outData, bool isInit)
{
	// 목표 ui 데이터
	FTextUIData objUIData;
	// 상세 ui 데이터
	FTextUIData detailUIData;

	// 목표 단
	FString checkHeightStr = FString::FormatAsNumber(data.checkHeight * 3.281 / 100);
	FString curHeightStr = FString::FormatAsNumber(data.curHeight * 3.281 / 100);

	objUIData.headerText = FRichString(TEXT("편대 비행")).GetFormatString();
	objUIData.bodyTextAry = {
		FRichString(FString::Printf(TEXT("편대 비행 중 : %s"), *UJ_Utility::ToStringBool(data.checkFormation))).GetFormatString()
		,FRichString(FString::Printf(TEXT("목표 고도 %s ft: \n현재 %s ft"), *checkHeightStr, *curHeightStr)).GetFormatString()
		,FRichString(FString::Printf(TEXT("올바른 위치 : %s"), *UJ_Utility::ToStringBool(data.isCorrectPosition))).GetFormatString()
		,FRichString(FString::Printf(TEXT("당신의 역할 : %s"), *UJ_Utility::PilotRoleToString(data.pilotRole))).GetFormatString()
	};

	if(isInit)
	{
		// 상세 단
		detailUIData.headerText = FRichString(TEXT("임시 편대 상세 텍스트")).GetFormatString();
		detailUIData.bodyTextAry = {
			FRichString(TEXT("임시 상세 1")).GetFormatString()
			,FRichString(TEXT("doremi 상세 2")).GetFormatString()
			,FRichString(TEXT("임시 상세 3")).GetFormatString()
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
	objUIData.headerText = FRichString(TEXT("공대지 훈련")).GetFormatString();
	objUIData.bodyObjAry;

	// 서브 이동 목표 
	for(int i = 0; i < data.subMPSucceedDataAry.Num(); i++)
	{
		const auto& subObjData = data.subMPSucceedDataAry[i];
		FDefaultTextUIData subObjUI;

		// 헤더 텍스트
		FString text = FString::Printf(TEXT("%d번 웨이포인트 도달"), (i+1));
		// 완료된 목표면 성공 유무에 따라, 아직이면 기본
		ETextStyle style = subObjData.isEnd 
				? subObjData.isSuccess 
					? ETextStyle::SUCCESS 
					: ETextStyle::FAIL
				: ETextStyle::DEFAULT;

		// GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::White, FString::Printf(TEXT("%s"), ));
		UE_LOG(LogTemp, Warning, TEXT("%d : %s"), i,*UEnum::GetValueAsString(style));

		subObjUI.headerText = FRichString(text, style).GetFormatString(); 

		// 설명 텍스트
		switch(i)
		{
			// 첫 번째 이동 목표
			case 0:
			{
				subObjUI.bodyTextAry.Add(FRichString(TEXT("@@이동 목표 눈으로 확인하기"), ETextStyle::OBJDETAIL).GetFormatString());
			}
				break;
			case 1:
			{
				subObjUI.bodyTextAry.Add(FRichString(TEXT("@@3번 웨이포인트로 이동하는 동안 자신의 위치를 보고하는 임무를 수행하십시오."), ETextStyle::OBJDETAIL).GetFormatString());
			}
				break;
			case 3:
			{
				subObjUI.bodyTextAry.Add(FRichString(TEXT("@@미사일 발사 알림."), ETextStyle::OBJDETAIL).GetFormatString());
			}
				break;
		}

		objUIData.bodyObjAry.Add(subObjUI);
	}

	// 타격 목표 텍스트
	FDefaultTextUIData neutTarget;

	neutTarget.headerText = FRichString(TEXT("과녁 조준 및 미사일 발사")).GetFormatString();
	neutTarget.bodyTextAry.Add(FRichString(TEXT("목표물을 향해 미사일 발사"), ETextStyle::OBJDETAIL).GetFormatString());

	objUIData.bodyObjAry.Add(neutTarget);


	// objUIData.bodyTextAry.Add(FRichString(FString::Printf(TEXT("남은 지상 목표 %d/%d"), data.curTargetAmt, data.allTargetAmt)));

	// // 상세 단
	// if(isInit)
	// {
	// 	detailUIData.headerText = FRichString(TEXT("임시 지대공 상세 텍스트"));
	// 	detailUIData.bodyTextAry = {
	// 		FRichString(TEXT("임시 상세 1"))
	// 		,FRichString(TEXT("doremi 상세 2"))
	// 	};
	// }

	outData = TArray<FTextUIData> { objUIData , detailUIData};
}

void UJ_ObjectiveUIComp::CreateUIData(const FEngineProgressData &data, TArray<FTextUIData> &outData, bool isInit )
{
	// 목표 ui 데이터
	FTextUIData objUIData;
	// 상세 ui 데이터
	FTextUIData detailUIData;
	check(this);
	// 목표 단
	objUIData.headerText = FRichString(TEXT("시동 절차")).GetFormatString();
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
		
		// 본문에 추가
		// objUIData.bodyTextAry.Add(str);
		FDefaultTextUIData subObj;
		subObj.headerText = FRichString(data.ToStringProgressEnum(type), style).GetFormatString();
		subObj.bodyTextAry = {
			FRichString(TEXT("@@ 임시 시동설명텍스트"), ETextStyle::OBJDETAIL).GetFormatString()
		};

		objUIData.bodyObjAry.Add(subObj);
	}

	// 상세 단
	if(isInit)
	{
		detailUIData.headerText = FRichString(TEXT("임시 시동 절차 상세 텍스트")).GetFormatString();
		detailUIData.bodyTextAry = {
			FRichString(FString::Printf(TEXT("임시 텍스트 %d"), FMath::RandRange(1, 10))).GetFormatString()
			,FRichString(FString::Printf(TEXT("임시 텍스트 %d"), FMath::RandRange(1, 10))).GetFormatString()
		};
	}

	outData = TArray<FTextUIData> { objUIData , detailUIData};
}

void UJ_ObjectiveUIComp::CreateUIData(const FTakeOffData &data, TArray<FTextUIData> &outData, bool isInit)
{
	// 목표 ui 데이터
	FTextUIData objUIData;
	// 상세 ui 데이터
	FTextUIData detailUIData;
	check(this);
	// 목표 단
	objUIData.headerText = FRichString(TEXT("이륙 절차")).GetFormatString();

	FString str = FString::Printf(TEXT("@@ 임시 이륙 텍스트 : %d / %d"), data.curTakeOffCnt, data.maxTakeOffCnt);
	ETextStyle style = data.curTakeOffCnt <= data.maxTakeOffCnt ? ETextStyle::DEFAULT : ETextStyle::SUCCESS;
	objUIData.bodyTextAry.Add(FRichString(str,style).GetFormatString());

	outData = TArray<FTextUIData> { objUIData , detailUIData};
}
