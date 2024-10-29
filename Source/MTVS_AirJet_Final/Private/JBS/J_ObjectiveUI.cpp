// Fill out your copyright notice in the Description page of Project Settings.


#include "JBS/J_ObjectiveUI.h"
#include "Components/TextBlock.h"
#include "Styling/SlateColor.h"

void UJ_ObjectiveUI::SetObjHeaderText(const FString& value)
{
    // 텍스트 설정
    objHeaderText->SetText(FText::FromString(value));

}

void UJ_ObjectiveUI::SetObjBodyText(TArray<FString> valueAry)
{
    // @@ 일단 하나만
    objBodyText->SetText(FText::FromString(valueAry[0]));


    // @@ ary보다 child가 적을시 추가 생성
}

void UJ_ObjectiveUI::SetObjUI(FObjUIData data)
{
    SetObjHeaderText(data.headerText);
    SetObjBodyText(data.bodyTextAry);
}

void UJ_ObjectiveUI::EndSubObjUI(int idx, bool isSuccess)
{
    // @@ 완료 UMG
    
    // FIXME idx 사용하려면 VBox 가져와야함

    // 성공 유무에 따라 텍스트 색상 변경
    objBodyText->SetColorAndOpacity(isSuccess ? successTextColor : failTextColor);

    // @@ 취소선 처리?
}

void UJ_ObjectiveUI::EndObjUI(bool isSuccess)
{
    // XXX 성공 유무에 따라 텍스트 색상 변경 | UMG 에서 처리
    // objHeaderText->SetColorAndOpacity(isSuccess ? successTextColor : failTextColor);

    // @@ 취소선 처리?

    // @@ 완료 UMG
    PlayObjEndAnim();
    // umg 종료후 비활성화
}

void UJ_ObjectiveUI::StartObjUI()
{
    // 시작 UMG
    PlayObjStartAnim();

    objHeaderText->SetColorAndOpacity(defaultTextColor);
    objBodyText->SetColorAndOpacity(defaultTextColor);
}