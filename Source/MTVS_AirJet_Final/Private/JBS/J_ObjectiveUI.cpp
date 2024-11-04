// Fill out your copyright notice in the Description page of Project Settings.


#include "JBS/J_ObjectiveUI.h"
#include "Components/RichTextBlock.h"
#include "Components/SlateWrapperTypes.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/WidgetSwitcher.h"
#include "Engine/Engine.h"
#include "JBS/J_DetailUI.h"
#include "JBS/J_ObjectiveSubElementUI.h"
#include "JBS/J_ObjectiveTextUI.h"
#include "Styling/SlateColor.h"
#include "JBS/J_MissionCompleteUI.h"



void UJ_ObjectiveUI::SetObjUI(FTextUIData data, bool isInit)
{
    // ui 설정
    objectiveTextUI->SetTextUI(data, isInit);
}

void UJ_ObjectiveUI::SetObjUI(TArray<FTextUIData>& data, bool isInit)
{
    // ui 설정
    objectiveTextUI->SetTextUI(data[0], isInit);
    // 상세 ui 설정
    DETAIL_TEXT_UI->SetTextUI(data[1], isInit);
}



void UJ_ObjectiveUI::StartObjUI()
{
    // 시작 UMG 애니메이션 바인드 | settextui 이후 실행
    objectiveTextUI->startAnimDel.BindUObject(this, &UJ_ObjectiveUI::PlayObjStartAnim);

    // 글자 색 초기화
    ResetTextColor();
    
}

void UJ_ObjectiveUI::EndObjUI(bool isSuccess)
{
    // 완료 UMG
    PlayObjEndAnim(this->GetObjTextUI());

    // @@ 취소선 처리?
    // umg 종료후 비활성화 | 지금은 하드코딩으로 맞추고 있음
}

void UJ_ObjectiveUI::EndSubObjUI(int idx, bool isSuccess)
{
    // @@ 서브 완료 UMG
    
    // @@ 성공 유무에 따라 텍스트 색상 변경
    // FIXME
    // auto subUIs = objBodyElementVBox->GetAllChildren();
    // TArray<UJ_ObjectiveSubElementUI*> subEleUIs;
    // Algo::Transform(subUIs, subEleUIs, [](UWidget* temp){
    //     return Cast<UJ_ObjectiveSubElementUI>(temp);
    // });
    // //캐스트 후
    
    // // 유효 체크
    // if(subEleUIs.Num() <= idx)
    // {
    //     GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("EndSubObjUI : subUI out of range"));

    //     return;
    // }

    // FIXME 색상 변경
    // subEleUIs[idx]->objSubText->SetColorAndOpacity(isSuccess ? successTextColor : failTextColor);

    // @@ 취소선 처리?
}



void UJ_ObjectiveUI::ActiveResultUI(const TArray<FObjectiveData>& resultObjData)
{
    MissionUI_Switcher->SetActiveWidget(missionCompleteUI);
    
    // FString asd = FString::Printf(TEXT("결과 obj 개수 : %d"), resultObjData.Num());
    // GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::White, FString::Printf(TEXT("%s"), *asd));
    // UE_LOG(LogTemp, Warning, TEXT("%s"), *asd);

    missionCompleteUI->SetResultListValue(resultObjData);
}

void UJ_ObjectiveUI::ResetTextColor()
{
// objHeaderText->SetColorAndOpacity(defaultTextColor);

    // FIXME
    // auto allSub = objBodyElementVBox->GetAllChildren();
    // TArray<UJ_ObjectiveSubElementUI*> subUIs;
    // Algo::Transform(allSub, subUIs, [](UWidget* temp){
    //     return Cast<UJ_ObjectiveSubElementUI>(temp);
    // });
    // //캐스트 후
    // for(auto* subUI : subUIs)
    // {
    //     // FIXME 추가해야함
    //     // subUI->objSubText->SetColorAndOpacity(defaultTextColor);
    // }
    // objBodyText->SetColorAndOpacity(defaultTextColor);
}








// XXX
// void UJ_ObjectiveUI::SetObjHeaderText(const FString& value)
// {
//     // 텍스트 설정
//     objHeaderText->SetText(FText::FromString(value));
// }

// XXX
// void UJ_ObjectiveUI::SetObjBodyText(TArray<FString> valueAry)
// {
//     if (valueAry.Num() > objBodyElementVBox->GetChildrenCount()) {
//         GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("서브 조건 리스트 : 너무 많은 값 요청"));
        
//         return;
//     }
//     // solved valueAry의 개수 만큼 ui visible  | 나중엔 요소 생성하는 방향으로 수정
//     auto objBodies = objBodyElementVBox->GetAllChildren();
//     // 전체 비활성화
//     for(auto* subElement : objBodies)
//     {
//         subElement->SetVisibility(ESlateVisibility::Hidden);
//     }

//     // value 들어간 인덱스만큼 설정
//     for(int i = 0; i < valueAry.Num(); i++)
//     {
//         // solved 나중엔 객체지향식으로 변경

//         auto valueStr = valueAry[i];
//         UJ_ObjectiveSubElementUI* subUI = Cast<UJ_ObjectiveSubElementUI>(objBodies[i]);
//         objBodies[i]->SetVisibility(ESlateVisibility::Visible);

//         subUI->objSubText->SetText(FText::FromString(valueStr));
//     }
//     // objBodyText->SetText(FText::FromString(valueAry[0]));


//     // solved ary보다 child가 적을시 추가 생성
// }

