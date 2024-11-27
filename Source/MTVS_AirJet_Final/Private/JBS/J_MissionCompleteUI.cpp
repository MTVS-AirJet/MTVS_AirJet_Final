// Fill out your copyright notice in the Description page of Project Settings.


#include "JBS/J_MissionCompleteUI.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/SlateWrapperTypes.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "JBS/J_CircleProgressBar.h"
#include "JBS/J_GameInstance.h"
#include "JBS/J_JsonUtility.h"
#include "JBS/J_MissionCompleteObjElement.h"
#include "JBS/J_ObjectiveSubElementUI.h"
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

    // MC_SuccessGradeImage->SetVisibility(ESlateVisibility::Hidden);

    rankDel.AddDynamic( this, &UJ_MissionCompleteUI::PlayRankMotion);
}

void UJ_MissionCompleteUI::OnClickReturnLobby()
{
    // 리턴 로비 실행
    returnLobbyDel.Broadcast();
}

void UJ_MissionCompleteUI::SetResultListValue(const TArray<FObjectiveData> &resultObjData)
{
    

    // 데이터 가지고 수행도 섹션 값 설정
    FTextUIData successData;
    successData.headerText = FRichString(TEXT("이번 직업체험에서 배운것"), ETextStyle::RESULTHEADER).GetFormatString();
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

        FString bodyTextResult = FRichString(FString::Printf(TEXT("%s %s"), *bodyText, *UJ_Utility::ToStringPercent(data.successPercent)), ETextStyle::RESULTDEFAULT).GetFormatString();
        // 수행도 텍스트 설정
        successData.bodyTextAry.Add(bodyTextResult);
    }

    MC_SuccessTextUI->SetTextUI(successData, true);

    MC_SuccessTextUI->SetAllChildrenSize(0.0f);


    // ai 피드백
    FTextUIData aic;
    aic.headerText = FRichString(TEXT("AI 비행 분석"), ETextStyle::RESULTHEADER).GetFormatString();
    aic.bodyTextAry = {
        FRichString(TEXT("AI 피드백 작성중..."), ETextStyle::RESULTDEFAULT).GetFormatString()
    };

    MC_AICommentTextUI->SetTextUI(aic, true);

    MC_AICommentTextUI->SetAllChildrenSize(0.f);

    // 수행도 배열
    TArray<float> successPercentAry;
    Algo::Transform(resultObjData, successPercentAry, [](FObjectiveData temp){
        return temp.successPercent;
    });
    // 수행도 평균 계산
    spAvgValue = UJ_Utility::CalcAverage(successPercentAry);
 
    spAvgData.headerText = FRichString(TEXT("수행도"), ETextStyle::RESULTHEADER).GetFormatString();
    spAvgData.bodyTextAry = {
        FRichString(TEXT("총점 : - %"), ETextStyle::RESULTDEFAULT).GetFormatString()
    };

    MC_SuccessAvgTextUI->SetTextUI(spAvgData, true);


    PlayStartAnim();
}

void UJ_MissionCompleteUI::SetSuccessAvgText(int percent)
{
    // 수행도 바디 텍스트 변경
    spAvgData.bodyTextAry = {
        FRichString(FString::Printf(TEXT("총점 : %d %%"), percent), ETextStyle::RESULTDEFAULT).GetFormatString()
    };

    MC_SuccessAvgTextUI->SetTextUI(spAvgData, false);
}

void UJ_MissionCompleteUI::SetAIFeedback(const FAIFeedbackRes &resData)
{
    // ai 피드백
    aiComment = resData.comment;
    dataRank = resData.rank;

    isGetData = true;

    rankDel.Broadcast();

    // PlayResultGrade(resData.rank);
}

void UJ_MissionCompleteUI::SetAIComment()
{
    FTextUIData aic;
    aic.headerText = FRichString(TEXT("AI 비행 분석"), ETextStyle::RESULTHEADER).GetFormatString();
    aic.bodyTextAry = {
        FRichString(aiComment, ETextStyle::RESULTDEFAULT).GetFormatString()
    };

    MC_AICommentTextUI->SetTextUI(aic, false);

    MC_AICommentTextUI->SetAllChildrenSize(0.0f);
}

void UJ_MissionCompleteUI::PlayResultGrade(int rank)
{
    // ai 랭크
    if(rank >= 0 && rank < gradeImgAry.Num())
    {
        MC_SuccessGradeImage->SetBrush(gradeImgAry[rank]);
    }
    else {
        GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("등급 값이 이상해요 : %d"), rank));
    }

    PlayGradeAnim();
}

void UJ_MissionCompleteUI::InitValue()
{
    MC_SuccessValueUI->SetPercent(0.f);
    MC_SuccessGradeImage->SetVisibility(ESlateVisibility::Hidden);
    // 수행도 섹션 안보이게
    auto allSPSection = MC_SuccessTextUI->GetBodyVBox()->GetAllChildren();
    for(auto* spSection : allSPSection)
    {
        spSection->SetRenderScale(FVector2D(0,0));
    }
    // 수행도 애니메이션 종료 시 이벤트 바인드
    MC_SuccessTextUI->animEndDel.AddDynamic(this, &UJ_MissionCompleteUI::EventSPAnimEnd);
}