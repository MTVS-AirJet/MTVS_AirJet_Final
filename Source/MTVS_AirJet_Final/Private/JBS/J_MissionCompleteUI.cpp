// Fill out your copyright notice in the Description page of Project Settings.


#include "JBS/J_MissionCompleteUI.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "JBS/J_MissionCompleteObjElement.h"
#include "JBS/J_Utility.h"
#include "Layout/Margin.h"
#include "Styling/SlateBrush.h"
#include "UObject/Class.h"
#include "JBS/J_ObjectiveTextUI.h"


void UJ_MissionCompleteUI::NativeConstruct()
{
    Super::NativeConstruct();

    // 버튼 함수 바인드
    MC_BackToLobbyBtn->OnClicked.AddDynamic( this, &UJ_MissionCompleteUI::OnClickReturnLobby);

    UE_LOG(LogTemp, Warning, TEXT("%s"), *UEnum::GetValueAsString(ETextStyle::DEFAULT));
}

void UJ_MissionCompleteUI::OnClickReturnLobby()
{
    // 리턴 로비 실행
    returnLobbyDel.ExecuteIfBound();
}

void UJ_MissionCompleteUI::SetResultListValue(const TArray<FObjectiveData> &resultObjData)
{
    // 데이터 가지고 수행도 섹션 값 설정
    FTextUIData successData;
    successData.headerText = FRichString(TEXT("클리어 지표"), ETextStyle::RESULTHEADER).GetFormatString();
    for(const auto& data : resultObjData)
    {
        FString bodyText = "";
        switch (data.objType) 
        {
            case ETacticalOrder::NONE:
                break;
            case ETacticalOrder::MOVE_THIS_POINT:
                break;
            case ETacticalOrder::FORMATION_FLIGHT:
                bodyText = TEXT("편대 비행");
                break;
            case ETacticalOrder::NEUTRALIZE_TARGET:
                bodyText = TEXT("공대지 훈련");
                break;
            case ETacticalOrder::ENGINE_START:
                bodyText = TEXT("시동");
                break;
            case ETacticalOrder::TAKE_OFF:
                bodyText = TEXT("이륙");
                break;
        }

        FString bodyTextResult = FString::Printf(TEXT("%s %s"), *bodyText, *UJ_Utility::ToStringPercent(data.successPercent));
        // 수행도 텍스트 설정
        successData.bodyTextAry.Add(bodyTextResult);
    }

    MC_SuccessTextUI->SetTextUI(successData, true);

    // ai 피드백
    FTextUIData aiComment;
    aiComment.headerText = FRichString(TEXT("AI 비행 분석"), ETextStyle::RESULTHEADER).GetFormatString();
    aiComment.bodyTextAry = {
        FRichString(TEXT("AI 피드백 입력중..."), ETextStyle::RESULTDEFAULT).GetFormatString()
    };

    // FIXME 내부 내용 AI와 통신

    MC_AICommentTextUI->SetTextUI(aiComment, true);

    // @@ 수행도 | 나중엔 그냥 구조체에 포함시키기
    TArray<float> spAry;
    Algo::Transform(resultObjData, spAry, [](FObjectiveData temp){
        return temp.successPercent;
    });
    //캐스트 후
    float avg = UJ_Utility::CalcAverage(spAry);

    FTextUIData successAvg;
    successAvg.headerText = FRichString(TEXT("수행도"), ETextStyle::RESULTHEADER).GetFormatString();

    MC_SuccessAvgTextUI->SetTextUI(successAvg, true);

    // FIXME 결과 등급 AI 계산


    // MC_SuccessGradeImage->SetBrush(FSlateBrush asd)
}