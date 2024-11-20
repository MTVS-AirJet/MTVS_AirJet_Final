#include "JBS/J_DetailUI.h"
#include "Components/Image.h"
#include "JBS/J_Utility.h"



void UJ_DetailUI::SetDetailUI(int idx)
{
    //idx 보정
    
    // enum으로 변환
    const auto enumValue = static_cast<EMissionProcess>(idx);
    
    // 유효 값 확인
    bool isValid = EMissionProcess::MIC_SWITCH_ON <= enumValue && enumValue <= EMissionProcess::MISSION_END;
    if(!isValid) return;
    
    SetDetailUI(enumValue);
}

void UJ_DetailUI::AdjustIdx(int &idx)
{
    // 특수 인덱스 값 보정 | 같은 이미지를 쓴다던지
    switch(idx)
    {
        case 0:
        default:
            break;
    }
}

void UJ_DetailUI::SetDetailUI(const EMissionProcess& value)
{
    if(!imgMap.Contains(value)) return;
    // 낭비 방지 처리
    if(curImgKey == value) return;

    // 이미지 변경
    detailImage->SetBrush(imgMap[value]);
    curImgKey = value;
}