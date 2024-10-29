// Fill out your copyright notice in the Description page of Project Settings.


#include "JBS/J_ObjectiveIconUI.h"
#include "Components/TextBlock.h"

void UJ_ObjectiveIconUI::SetObjDisText(float distance)
{
    // distance는 cm 단위로 들어옴
    float meter = distance / 100.f;
    // 단위 str
    FString meterStr;

    if(meter >= 1000)
    {
        meter /= 1000;
        meterStr = TEXT("KM");
    }
    else {
        meterStr = TEXT("M");
    }

    // 미터 넣기
    FString result = FString::Printf(TEXT("%.1f %s"), meter, *meterStr);

    objDisText->SetText(FText::FromString(result));
}