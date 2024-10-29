// Fill out your copyright notice in the Description page of Project Settings.


#include "JBS/J_PilotViewTabletUI.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/Spacer.h"
#include "Components/HorizontalBox.h"
#include "Components/SlateWrapperTypes.h"
#include "Components/VerticalBox.h"
#include "Components/WidgetSwitcher.h"
#include "JBS/J_PlayerSlotUI.h"
#include "Templates/Casts.h"
#include "Types/SlateEnums.h"
#include "UObject/Linker.h"
#include "Components/HorizontalBoxSlot.h"
#include "UObject/UObjectGlobals.h"

void UJ_PilotViewTabletUI::NativeConstruct()
{
    Super::NativeConstruct();

    PV_ScreenBtn->OnClicked.AddDynamic( this, &UJ_PilotViewTabletUI::OnClickGoSelectScreen);
}

void UJ_PilotViewTabletUI::OnClickGoSelectScreen()
{
    PilotViewWidgetSwitcher->SetActiveWidgetIndex(1);
}

void UJ_PilotViewTabletUI::AddPlayerSlot(const FString& playerId)
{
    // 현재 row 가 없거나 child 가 2개로 꽉 찼다면 새 hbox 생성
    if(curHBox == nullptr) 
    {
        curHBox = CreateNewPlayerSlotHBox();
        // GEngine->AddOnScreenDebugMessage(-1, 32.f, FColor::Green, TEXT("새 hbox 생성 1"));
    }
         

    bool hBoxFull = false;
    // hBox가 슬롯으로 꽉차있는지 확인
    auto hBoxChildren = curHBox->GetAllChildren();
    int slotCnt = 0;
    for(auto* child : hBoxChildren)
    {
        if(child->IsA<UJ_PlayerSlotUI>())
            slotCnt++;
    }
    if(slotCnt >= playerSlotRowSize)
    {
        curHBox = CreateNewPlayerSlotHBox();
        // GEngine->AddOnScreenDebugMessage(-1, 32.f, FColor::Green, TEXT("새 hbox 생성 2"));
    } 

    
    // hbox 가 비어있는지 확인
    bool hBoxEmpty = curHBox->GetChildrenCount() == 0;
    bool hasSpace = false;
    int spaceIdx = 0;
    // 자식 중에 spacer가 있는지 확인
    for(spaceIdx; spaceIdx < curHBox->GetChildrenCount(); spaceIdx++)
    {
        auto* childUI = curHBox->GetChildAt(spaceIdx);
        check(childUI);
        if(childUI->IsA<USpacer>())
        {
            hasSpace = true;
            break;
        }
    }
    
    // 플레이어 슬롯 ui 생성
    auto* newPlayerSlot = CastChecked<UJ_PlayerSlotUI>(CreateWidget(GetWorld(), playerSlotUIPrefab));
    check(newPlayerSlot);
    // hbox에 붙이기
    auto* slot = CastChecked<UHorizontalBoxSlot>(curHBox->AddChildToHorizontalBox(newPlayerSlot));
    SetHorizontalSlot(slot, EHorizontalAlignment::HAlign_Fill, 1.0f / playerSlotRowSize);

    // hbox가 원래 비어있었다면 빈 공간 만큼 spacer 생성
    if(hBoxEmpty)
    {
        // GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("스페이서 생성"));
        for(int i = 0; i < playerSlotRowSize -1; i++)
        {
            auto* tempSlot = CastChecked<UHorizontalBoxSlot>(curHBox->AddChildToHorizontalBox(NewObject<USpacer>(this)));
            SetHorizontalSlot(tempSlot, EHorizontalAlignment::HAlign_Fill, 1.0f / playerSlotRowSize);
        }
    }
    // spacer가 있다면 하나 제거
    else
    {
        // GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("제거"));
        curHBox->RemoveChildAt(spaceIdx);
    }

    // XXX 딜리게이트, text 설정 등 init 하기
}

void UJ_PilotViewTabletUI::AddPlayerSlot(TArray<FString> playerIdAry)
{
    // GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString::Printf(TEXT("%d 번 생성할게"), tempPlayerIds.Num()));
    // XXX 테스트용으로 자체 변수 사용
    for(FString playerId : tempPlayerIds)
    {
        AddPlayerSlot(playerId);
    }
}

UHorizontalBox *UJ_PilotViewTabletUI::CreateNewPlayerSlotHBox()
{
    
    // 새 hbox 만들어서 slot root 에 붙이고 반환
    // 새 hbox 생성
    auto* newHBox = NewObject<UHorizontalBox>(this);
    // col 에 붙이기
    PVS_SlotColumn->AddChildToVerticalBox(newHBox);

    return newHBox;
}


void UJ_PilotViewTabletUI::SetHorizontalSlot(UHorizontalBoxSlot *hSlot, EHorizontalAlignment align, float sizeValue)
{
    if(!hSlot) return;
    
    hSlot->SetHorizontalAlignment(align);
    auto tempSize = FSlateChildSize();
    tempSize.Value = sizeValue;

    hSlot->SetSize(tempSize);
}