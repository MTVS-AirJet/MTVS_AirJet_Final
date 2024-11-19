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

void UJ_ObjectiveUIComp::CRPC_StartObjUIEngine_Implementation(const FEngineProgressData& orderData)
{
	// 비활성 상태시 활성화
	if(objUI->GetVisibility() == ESlateVisibility::Hidden)
		objUI->SetVisibility(ESlateVisibility::Visible);

	// 시작 처리
	objUI->StartObjUI();
	// 값 설정
	CRPC_UpdateObjUIEngine(orderData, true);
}

void UJ_ObjectiveUIComp::CRPC_UpdateObjUIEngine_Implementation(const FEngineProgressData &orderData, bool isInit)
{
	TArray<FTextUIData> textUIData;

	CreateUIData(orderData, textUIData, isInit);

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

void UJ_ObjectiveUIComp::CRPC_StartObjUITakeOff_Implementation(const FTakeOffData &orderData)
{
	// 시작 처리
	objUI->StartObjUI();
	// 값 설정
	CRPC_UpdateObjUITakeOff(orderData, true);
}

void UJ_ObjectiveUIComp::CRPC_UpdateObjUITakeOff_Implementation(const FTakeOffData &orderData, bool isInit)
{
	TArray<FTextUIData> textUIData;

	CreateUIData(orderData, textUIData, isInit);

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

void UJ_ObjectiveUIComp::CRPC_StartObjUIFormation_Implementation(const FFormationFlightUIData &orderData)
{
	// 시작 처리
	objUI->StartObjUI();
	// 값 설정
	CRPC_UpdateObjUIFormation(orderData, true);
}

void UJ_ObjectiveUIComp::CRPC_UpdateObjUIFormation_Implementation(const FFormationFlightUIData &orderData, bool isInit)
{
	TArray<FTextUIData> textUIData;

	CreateUIData(orderData, textUIData, isInit);

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

void UJ_ObjectiveUIComp::CRPC_StartObjUINeut_Implementation(const FNeutralizeTargetUIData &orderData)
{
	// 시작 처리
	objUI->StartObjUI();
	// 값 설정
	CRPC_UpdateObjUINeut(orderData, true);
}

void UJ_ObjectiveUIComp::CRPC_UpdateObjUINeut_Implementation(const FNeutralizeTargetUIData &orderData, bool isInit)
{
	TArray<FTextUIData> textUIData;

	CreateUIData(orderData, textUIData, isInit);

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


// XXX 현재 미사용
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
	// 고도 체크 서브
	// FIXME 이거 uidata 에서 애초에 받는걸로 변경하기
	float checkHeightAdj = 60000;
	bool isHeightPass = UJ_Utility::CheckValueRange(data.curHeight, data.checkHeight - checkHeightAdj, data.checkHeight + checkHeightAdj);
	ETextStyle heightStyle = isHeightPass ? ETextStyle::OBJDETAILSUCCESS : ETextStyle::OBJDETAILFAIL;
	// 포매팅
	FString formCurHeightStr = FRichString(curHeightStr, heightStyle).GetFormatString();

	FString rCHS1 = FRichString(FString::Printf(TEXT("지정된 고도 %s 피트를 유지합니다."), *checkHeightStr), ETextStyle::OBJDETAIL).GetFormatString();
	
	FString rCHS2 = FRichString(TEXT("- 현재 "), ETextStyle::OBJDETAIL).GetFormatString()
		+ formCurHeightStr
		+ FRichString(TEXT(" ft"), ETextStyle::OBJDETAIL).GetFormatString();

	FDefaultTextUIData checkHeightData;
	checkHeightData.headerText = FRichString(TEXT("고도 유지")).GetFormatString();
	checkHeightData.bodyTextAry = {
		rCHS1 + "\n" + rCHS2
	};

	objUIData.bodyObjAry.Add(checkHeightData);

	// /

	// // 위치 서브
	// FDefaultTextUIData checkPosData;
	// checkPosData.headerText = FRichString(TEXT("편대 거리 유지")).GetFormatString();

	// objUIData.bodyObjAry.Add(checkPosData);
	
	// /

	

	// 나의 역할 : %s
	FDefaultTextUIData roleData;
	FRichString formRoleStr(FString::Printf(TEXT("나의 역할 : %s"), *data.ToStringPilotRolt()), ETextStyle::DEFAULT);

	roleData.headerText = formRoleStr.GetFormatString();

		// 역할 설명
	// |편대장은 빈칸
	// 편대장의 우측 후미에 붙어서 10 ft 간격을 유지한채로 비행해야합니다.
	FString roleDetail = data.pilotRole == EPilotRole::WING_COMMANDER ? TEXT("") : TEXT("윙맨은 편대장의 우측 후미에 붙어서 10 피트의 간격을 유지한채로 비행해야합니다.");
	FRichString formRoleDetail(roleDetail, ETextStyle::OBJDETAIL);

	// \n

		// 팀의 대형 유지 여부 :
	FRichString tf1(TEXT("팀의 대형 유지 여부 : "), ETextStyle::OBJDETAIL);
	// +
	FString isFormationstr = UJ_Utility::ToStringBool(data.isCorrectPosition);
	ETextStyle tf2Style = data.isCorrectPosition ? ETextStyle::OBJDETAILSUCCESS : ETextStyle::OBJDETAILFAIL;

	FRichString tf2(UJ_Utility::ToStringBool(data.isCorrectPosition), tf2Style);

	FString teamFormationStr = FString::Printf(
		TEXT("%s %s")
		, *tf1.GetFormatString(), *tf2.GetFormatString());

	FString totalRoleStr = teamFormationStr + "\n" + formRoleDetail.GetFormatString();

	roleData.bodyTextAry.Add(totalRoleStr);

	objUIData.bodyObjAry.Add(roleData);

	// 현재 편대 비행 중 | 편대 비행 조건 미충족 : 원인
	FDefaultTextUIData formationData;
	FRichString formHeader;
	if(data.checkFormation && isHeightPass)
	{
		formHeader.value = TEXT("현재 편대 비행 중");
		formHeader.styleType = ETextStyle::SUCCESS;

		formationData.bodyTextAry.Add(" ");

	}
	else {
		formHeader.value = TEXT("편대 비행 조건 미충족");
		formHeader.styleType = ETextStyle::FAIL;

		// 원인
		// isHeightPass, data.isCorrectPosition
		TArray<FString> formAry;
		if(!isHeightPass)
		{
			formAry.Add(FRichString (TEXT("고도 유지"), ETextStyle::OBJDETAILFAIL).GetFormatString());
		}
		if(!data.isCorrectPosition)
		{
			formAry.Add(FRichString (TEXT("진형 유지"), ETextStyle::OBJDETAILFAIL).GetFormatString());
		}
		FString resultFromStr = "";
		for(const FString& str : formAry)
		{
			resultFromStr += str + ", ";
		}
		if(!resultFromStr.IsEmpty())
			resultFromStr.RemoveAt(resultFromStr.Len() - 2, 2);
		
		formationData.bodyTextAry = {
			resultFromStr
		};
	}

	formationData.headerText = formHeader.GetFormatString();

	objUIData.bodyObjAry.Add(formationData);




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

		subObjUI.headerText = FRichString(text, style).GetFormatString(); 

		// 설명 텍스트
		switch(i)
		{
			// 첫 번째 이동 목표
			case 0:
			{
				subObjUI.bodyTextAry.Add(FRichString(TEXT("지정된 목표를 눈으로 확인합니다."), ETextStyle::OBJDETAIL).GetFormatString());
			}
				break;
			case 1:
			{
				subObjUI.bodyTextAry.Add(FRichString(TEXT("3번 웨이포인트로 가는 길에 두 번 위치를 보고합니다."), ETextStyle::OBJDETAIL).GetFormatString());
			}
				break;
			case 3:
			{
				subObjUI.bodyTextAry.Add(FRichString(TEXT("4번 웨이포인트 도달 직전에 미사일을 발사할 것을 알립니다."), ETextStyle::OBJDETAIL).GetFormatString());
			}
				break;
		}

		objUIData.bodyObjAry.Add(subObjUI);
	}

	// 타격 목표 텍스트
	FDefaultTextUIData neutTarget;

	FString cnt = FString::Printf(TEXT("과녁 조준 및 미사일 발사 ( %d / %d )"), data.curTargetAmt, data.allTargetAmt);


	neutTarget.headerText = FRichString(cnt).GetFormatString();
	neutTarget.bodyTextAry.Add(FRichString(TEXT("목표물을 향해 미사일을 발사합니다."), ETextStyle::OBJDETAIL).GetFormatString());

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
		// @@ 좀 효율적으로 바꿀 필요 있음
		// 엑셀 시트에서 불러오면 좋겠다
		FString textStr = "";
		switch (type) {
			case EEngineProgress::None:
				textStr = "";
				break;
			case EEngineProgress::MIC_SWITCH_ON:
				textStr = TEXT("지상 및 항공관제와 통신을 위해 무전기를 켭니다.");
				break;
			case EEngineProgress::ENGINE_GEN_SWITCH_ON:
				textStr = TEXT("엔진을 작동시키기 위해 전력 공급을 시작합니다.");
				break;
			case EEngineProgress::ENGINE_CONTROL_SWITCH_ON:
				textStr = TEXT("엔진의 작동을 조절할 수 있도록 제어 장치를 켭니다.");
				break;
			case EEngineProgress::ENGINE_MASTER_SWITCH_ON:
				textStr = TEXT("엔진의 모든 주요 시스템을 활성화합니다.");
				break;
			case EEngineProgress::JFS_STARTER_SWITCH_ON:
				textStr = TEXT("JFS(보조 동력 장치)는 엔진 시동을 위한 필수 장치입니다. 이 장치를 켜서 주 엔진을 시작할 준비를 합니다.");
				break;
			case EEngineProgress::JFS_HANDLE_PULL:
				textStr = TEXT("JFS(보조 동력 장치) 을 당기면, 주 엔진이 실제로 회전하기 시작합니다.");
				break;
			case EEngineProgress::ENGINE_THROTTLE_IDLE:
				textStr = TEXT("엔진이 천천히 회전하도록 합니다. 이때, 안정된 시동 상태를 유지합니다.");
				break;
			case EEngineProgress::CLOSE_CANOPY:
				textStr = TEXT("조종석 덮개를 닫습니다. 충분히 닫히면, 캐노피를 고정하여 외부와 격리된 비행 준비가 완료됩니다.");
				break;
			case EEngineProgress::STANDBY_OTHER_PLAYER:
				textStr = TEXT("다른 플레이어의 시동 절차가 완료될때까지 기다려주세요.");
				break;
			case EEngineProgress::RELEASE_SIDE_BREAK:
				textStr = TEXT("이륙 전 브레이크를 풀어 기체를 이륙시킬 준비를 합니다.");
				break;
			case EEngineProgress::TAKE_OFF:
				textStr = TEXT("엔진 출력을 80%로 높여 활주로를 따라 이륙을 시작합니다.");
				break;
		}
		subObj.bodyTextAry = {
			FRichString(textStr, ETextStyle::OBJDETAIL).GetFormatString()
		};

		objUIData.bodyObjAry.Add(subObj);
	}

	// 상세 단
	// 시동 절차 -> 미션 진행 인덱스로 변환해서 보내기
	objUIData.detailImgIdx = UJ_Utility::ConvertEngineProgressToMissionProcessIdx(data.curProgress);

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
	objUIData.headerText = FRichString(TEXT("이륙 시작")).GetFormatString();

	// 서브 목표 단
	ETextStyle style = data.curTakeOffCnt <= data.maxTakeOffCnt ? ETextStyle::DEFAULT : ETextStyle::SUCCESS;

	FDefaultTextUIData subObj;
	subObj.headerText = FRichString(FString::Printf(TEXT("이륙 수행도 : %d / %d"), data.curTakeOffCnt, data.maxTakeOffCnt), style).GetFormatString();
	subObj.bodyTextAry.Add(FRichString(TEXT("엔진 출력을 80%로 높여 활주로를 따라 이륙을 시작합니다."), ETextStyle::OBJDETAIL).GetFormatString());

	objUIData.bodyObjAry.Add(subObj);

	outData = TArray<FTextUIData> { objUIData , detailUIData};
}


