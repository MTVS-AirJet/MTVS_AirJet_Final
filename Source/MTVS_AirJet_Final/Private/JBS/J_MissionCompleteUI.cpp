// Fill out your copyright notice in the Description page of Project Settings.


#include "JBS/J_MissionCompleteUI.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/SlateWrapperTypes.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "JBS/J_GameInstance.h"
#include "JBS/J_JsonUtility.h"
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

    // MC_SuccessGradeImage->SetVisibility(ESlateVisibility::Hidden);
}

void UJ_MissionCompleteUI::OnClickReturnLobby()
{
    // 리턴 로비 실행
    returnLobbyDel.ExecuteIfBound();
}

void UJ_MissionCompleteUI::SetResultListValue(const TArray<FObjectiveData> &resultObjData)
{
    // AI 피드백 함수 바인드
    auto* gi = UJ_Utility::GetJGameInstance(GetWorld());
    gi->aiFeedbackResUseDel.BindUObject(this, &UJ_MissionCompleteUI::SetAIFeedback);

    TArray<float> successPercentAry;
    Algo::Transform(resultObjData, successPercentAry, [](FObjectiveData temp){
        return temp.successPercent;
    });
    //캐스트 후
    
    FAIFeedbackReq feedbackReq(successPercentAry);
    // 서버에 피드백 요청
    UJ_JsonUtility::RequestExecute(GetWorld(), EJsonType::AI_FEEDBACK, feedbackReq);


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

    MC_AICommentTextUI->SetTextUI(aiComment, true);

    


    float avg = UJ_Utility::CalcAverage(successPercentAry);
 
    FTextUIData successAvg;
    successAvg.headerText = FRichString(TEXT("수행도"), ETextStyle::RESULTHEADER).GetFormatString();
    successAvg.bodyTextAry = {
        FRichString(FString::Printf(TEXT("총점 : %d %%"), static_cast<int>(avg*100)), ETextStyle::RESULTDEFAULT).GetFormatString()
    };

    MC_SuccessAvgTextUI->SetTextUI(successAvg, true);
}

void UJ_MissionCompleteUI::SetAIFeedback(const FAIFeedbackRes &resData)
{
    // ai 피드백
    FTextUIData aiComment;
    aiComment.headerText = FRichString(TEXT("AI 비행 분석"), ETextStyle::RESULTHEADER).GetFormatString();
    aiComment.bodyTextAry = {
        FRichString(resData.comment, ETextStyle::RESULTDEFAULT).GetFormatString()
    };

    MC_AICommentTextUI->SetTextUI(aiComment, true);

    PlayResultGrade(resData.rank);
}

void UJ_MissionCompleteUI::PlayResultGrade(int rank)
{
    // ai 랭크
    if(rank >= 0 && rank < gradeImgAry.Num())
    {
        MC_SuccessGradeImage->SetBrush(gradeImgAry[rank]);
        MC_SuccessGradeImage->SetVisibility(ESlateVisibility::Visible);
        
    }
    else {
        GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("등급 값이 이상해요 : %d"), rank));
    }
}