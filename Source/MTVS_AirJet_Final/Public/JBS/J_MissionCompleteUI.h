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
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FReturnToLobbyDelegate);

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

	// 전체 수행도 원형 바
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|UI", meta = (BindWidget))
	class UJ_CircleProgressBar* MC_SuccessValueUI;

	// 수행 등급 이미지
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|UI", meta = (BindWidget))
	class UImage* MC_SuccessGradeImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Classes")
	TArray<FSlateBrush> gradeImgAry;
	// 

	// // 수행 결과 요소 프리팹
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Classes")
	// TSubclassOf<class UJ_MissionCompleteObjElement> objElementPrefab;

	// 총 수행도 평균
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Values")
	float spAvgValue = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Values")
	FTextUIData spAvgData;

	// ai 코멘트
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Values")
	FString aiComment;
	
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

	// 수행도 애니메이션 종료 이벤트
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void EventSPAnimEnd();

	// 수행도 평균 텍스트 설정
	UFUNCTION(BlueprintCallable)
	void SetSuccessAvgText(int percent);

        // 결과 등급 등장
	UFUNCTION(BlueprintCallable)
	void PlayResultGrade(int rank);

	UFUNCTION(BlueprintCallable)
	void SetAIComment();

	// 2. 결과 UI 시작 값 초기화
	UFUNCTION(BlueprintCallable)
	void InitValue();

    public:
	// ai 피드백 받기
	UFUNCTION(BlueprintCallable)
	void SetAIFeedback(const FAIFeedbackRes &resData);

	// 결과 값 받기
	// 결과 리스트 UI에 값 설정
    void SetResultListValue(const TArray<FObjectiveData> &resultObjData);

	// 시작 애니메이션 실행
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void PlayStartAnim();

	// 등급 애니메이션 실행
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void PlayGradeAnim();

	
};
