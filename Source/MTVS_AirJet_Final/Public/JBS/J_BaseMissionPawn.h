// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "J_BaseMissionPawn.generated.h"

UCLASS()
class MTVS_AIRJET_FINAL_API AJ_BaseMissionPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AJ_BaseMissionPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
protected:
	// 세션 아이디
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
	FString streamId;

	// 스트리밍 액터
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|Objects")
	class AK_StreamingActor* streamActor;
		public:
	__declspec(property(get = GetStreamActor, put = SetStreamActor)) class AK_StreamingActor* STREAM_ACTOR;
	class AK_StreamingActor *GetStreamActor();
	void SetStreamActor(class AK_StreamingActor* value)
	{
		streamActor = value;
	}
		protected:

	// 수신용 서버 url
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Default|Values")
	FString psServerURL = TEXT("ws://125.132.216.190:7755");

public:
	// FIXME 나중에 분리 필요
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Default|UI")
	class UK_StreamingUI* streamingUI;


protected:
    virtual void PossessedBy(AController *newController) override;

	// 화면 공유 시작
    virtual void StartScreenShare();
	// 온라인 세션 id 가져오기
    virtual FString GetSessionId();

public:
	// 픽셀 스트리밍용 함수
	UFUNCTION(Server, Reliable)
	void ServerRPC_SetStreamingPlayer(const FString &playerId, bool bAddPlayer);
};
