// Fill out your copyright notice in the Description page of Project Settings.


#include "JBS/J_MissionCompleteUI.h"
#include "Blueprint/UserWidget.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "JBS/J_MissionCompleteObjElement.h"
#include "JBS/J_Utility.h"
#include "Layout/Margin.h"

void UJ_MissionCompleteUI::SetResultListValue(const TArray<FObjectiveData> &resultObjData)
{
    for(auto data : resultObjData)
    {
        auto* newObjEle = CreateWidget<UJ_MissionCompleteObjElement>(GetWorld(), objElementPrefab);
        if(!newObjEle) continue;

        FString header = UJ_Utility::TacticalOrderToString(data.objType);
        FString body = FString::Printf(TEXT("%d %%"), static_cast<int32>(data.successPercent * 100));

        newObjEle->SetObjText(header, body);

        objListVBox->AddChildToVerticalBox(newObjEle);

        UVerticalBoxSlot* slot = Cast<UVerticalBoxSlot>(newObjEle->Slot);
        if(slot)
            slot->SetPadding(FMargin(0,0,0,25));
    }
}