#include "JBS/J_DetailUI.h"
#include "Components/Image.h"
#include "Containers/Map.h"
#include "JBS/J_Utility.h"
#include "Styling/SlateBrush.h"



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
    // 낭비 방지 처리
    if(curImgKey == value) return;

    // 이미지 변경
    SetDetailImg(value);
    curImgKey = value;
    // 갱신 애니메이션 실행
    PlayUpdateAnim();
}

void UJ_DetailUI::SetDetailImg(const EMissionProcess &value)
{
    detailImage->SetBrush(GetImg(value));
}

FSlateBrush UJ_DetailUI::GetImg(const EMissionProcess& process)
{
    const TMap<EMissionProcess, FSlateBrush>* imgMap;
    // 현재 입력장치에 따라 imgMap 선택
    switch (curInput) {
        case EInputDevice::KEYBOARD:
            imgMap = &imgMapKeyboard;
            break;
        case EInputDevice::FLIGHT_CONTROLLER:
            imgMap = &imgMapDefault;
            break;
        default:
            imgMap = &imgMapDefault;
            break;
    }

    if(!imgMap->Contains(process)) return FSlateBrush();
    // 이미지 반환
    return (*imgMap)[process];
}