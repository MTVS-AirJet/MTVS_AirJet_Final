// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "JBS/J_Utility.h"
#include "J_MissionCompleteUI.generated.h"

/**
 * 
 */
// pc에 로비로 돌아가기 바인드할 딜리게이트 추가
DECLARE_DELEGATE(FReturnToLobbyDelegate)

UCLASS()
class MTVS_AIRJET_FINAL_API UJ_MissionCompleteUI : public UUserWidget
{
	GENERATED_BODY()
protected:
	// // 수행 결과 리스트
	// UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|UI", meta = (BindWidget))
	// class UVerticalBox* objListVBox;

	// 로비 돌아가기 버튼
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|UI", meta = (BindWidget))
	class UButton* MC_BackToLobbyBtn;

	// 수행도 리스트 ui
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|UI", meta = (BindWidget))
	class UJ_ObjectiveTextUI* MC_SuccessTextUI;
	// AI 코멘트 ui
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|UI", meta = (BindWidget))
	class UJ_ObjectiveTextUI* MC_AICommentTextUI;
	// 전체 수행도 ui
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|UI", meta = (BindWidget))
	class UJ_ObjectiveTextUI* MC_SuccessAvgTextUI;

	// 수행 등급 이미지
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|UI", meta = (BindWidget))
	class UImage* MC_SuccessGradeImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Classes")
	TArray<FSlateBrush> gradeImgAry;
	// 

	// // 수행 결과 요소 프리팹
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Classes")
	// TSubclassOf<class UJ_MissionCompleteObjElement> objElementPrefab;
public:
	// 로비로 돌아가기 버튼 딜리게이트
	FReturnToLobbyDelegate returnLobbyDel;
	// 요소 추가
protected:

	// init
	virtual void NativeConstruct() override;

	// 로비로 돌아가기 이벤트
	UFUNCTION(BlueprintCallable)
	void OnClickReturnLobby();

	UFUNCTION(BlueprintCallable)
	void SetAIFeedback(const FAIFeedbackRes &resData);

	// 결과 등급 등장
	UFUNCTION(BlueprintCallable)
	void PlayResultGrade(int rank);

    public:
	// 결과 값 받기
	// 결과 리스트 UI에 값 설정
    void SetResultListValue(const TArray<FObjectiveData> &resultObjData);
};
