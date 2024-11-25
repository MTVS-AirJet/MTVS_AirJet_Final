// Fill out your copyright notice in the Description page of Project Settings.


#include "JBS/J_ObjectiveMovePoint.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/Engine.h"
#include "Engine/HitResult.h"
#include "JBS/J_BaseMissionObjective.h"
#include "JBS/J_BaseMissionPawn.h"
#include "JBS/J_MissionPlayerController.h"
#include "JBS/J_Utility.h"
#include "Math/MathFwd.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "TimerManager.h"

AJ_ObjectiveMovePoint::AJ_ObjectiveMovePoint() : AJ_BaseMissionObjective()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    // 성공 체크 캡슐 컴포넌트 설정
    checkCapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("checkCapsuleComp"));
    checkCapsuleComp->SetupAttachment(rootComp);
    checkCapsuleComp->SetCapsuleHalfHeight(40000);
    checkCapsuleComp->SetCapsuleRadius(4000);
    // 이펙트용 메시 설정
    beamMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("beamMeshComp"));
    beamMeshComp->SetupAttachment(rootComp);
    beamMeshComp->SetRelativeScale3D(FVector(40,40,20000));
    beamMeshComp->SetCollisionProfileName(FName(TEXT("NoCollision")));
}

#pragma region 시작 설정 단
/* MOVEPOINT는 하위 목표나 상속받아 편대비행으로 사용하므로 UI DATA 전송은 하지 않음
   따라서 딜리게이트 바인드 설정 | InitBindDel (편대비행은 따로 처리)
-> 성공 체크할 캡슐 overlap 함수 바인드
-> 목표 활성 유무에 따라 체크 캡슐 및 빔 이펙트 메시 활성화 동기화 | SetObjectiveActive
-> 목표 활성화시 실패 판정 방향 설정(편대장 기준) | ObjectiveActive, SetBaseDir
*/

void AJ_ObjectiveMovePoint::BeginPlay()
{
    Super::BeginPlay();

    // 체크 캡슐 값 가져와서 캐시
    beamLength = checkCapsuleComp->GetScaledCapsuleHalfHeight();
    beamRadius = checkCapsuleComp->GetScaledCapsuleRadius();
    
    beamMeshComp->SetRelativeScale3D(FVector(beamRadius / 100, beamRadius / 100, beamLength / 100));



    // 오버랩 바인드
    checkCapsuleComp->OnComponentBeginOverlap.AddDynamic(this, &AJ_ObjectiveMovePoint::OnCheckCapsuleBeginOverlap);
}

void AJ_ObjectiveMovePoint::InitBindDel()
{
    if(!HasAuthority()) return;

	// 활/비활성화 함수 바인드
	objectiveActiveDel.AddUObject(this, &AJ_ObjectiveMovePoint::ObjectiveActive);
	objectiveDeactiveDel.AddUObject(this, &AJ_ObjectiveMovePoint::ObjectiveDeactive);

    // 종료시 사운드 재생 바인드
    objectiveEndDel.AddDynamic( this, &AJ_ObjectiveMovePoint::PlayObjSound);
}

void AJ_ObjectiveMovePoint::PlayObjSound()
{
    const auto& allPC = UJ_Utility::GetAllMissionPC(GetWorld());
    for(auto* pc : allPC)
    {
        if(!pc) return;

        pc->CRPC_PlayObjSound(EObjSound::MOVE_POINT_END);
    }
}

void AJ_ObjectiveMovePoint::SetObjectiveActive(bool value)
{
    Super::SetObjectiveActive(value);
    
    // 빔 활/비활성화
    MRPC_SetVisibleBeam(value);
}

void AJ_ObjectiveMovePoint::MRPC_SetVisibleBeam_Implementation(bool value)
{
    // 충돌체 활/비활성화
    checkCapsuleComp->SetActive(value);
    FName profile = value ? FName(TEXT("MovePoint")) : FName(TEXT("NoCollision"));
    checkCapsuleComp->SetCollisionProfileName(profile);
    // 이펙트 메시 활/비활성화
    beamMeshComp->SetHiddenInGame(!value);
}

void AJ_ObjectiveMovePoint::ObjectiveActive()
{
    Super::ObjectiveActive();

    // 기준 방향 설정
    baseDirection = SetBaseDir();
}

FVector AJ_ObjectiveMovePoint::SetBaseDir()
{
    // 호스트를 기준으로 실패 판정 방향 설정
    // 편대장 위치 가져오기
    auto* localPawn = UJ_Utility::GetBaseMissionPawn(GetWorld());
    if(!localPawn) return this->GetActorForwardVector();

    const auto& leaderLoc = localPawn->GetActorLocation();

    // 편대장 -> 나 방향 반환
    return (this->GetActorLocation() - leaderLoc).GetSafeNormal();
}



#pragma endregion

#pragma region tick 목표 수행 단
/* tick 활성화시 실패 판정 체크 | CheckFail, ObjectiveEnd
-> 체크 캡슐에 충돌시 성공 처리 | OnCheckCapsuleBeginOverlap, ObjectiveEnd
*/

void AJ_ObjectiveMovePoint::Tick(float deltaTime)
{
    Super::Tick(deltaTime);

    // 활성화 중 실패 체크
    if(!HasAuthority() || IS_OBJ_ENDED) return;

    // 실패 체크
    if(CheckFail(baseDirection))
        this->ObjectiveEnd(false);
}

void AJ_ObjectiveMovePoint::OnCheckCapsuleBeginOverlap(
    UPrimitiveComponent *OverlappedComponent,
    AActor *OtherActor, UPrimitiveComponent *OtherComp,
    int32 OtherBodyIndex, bool bFromSweep,
    const FHitResult &SweepResult)
{
    // 활성화 상태일때만
    if(!HasAuthority()) return;
    if(IS_OBJ_ENDED) return;

    // 충돌한게 미션 폰이면 목표 성공 처리
    if(OtherActor->IsA<AJ_BaseMissionPawn>())
    {
        // FString debugStr = FString::Printf(TEXT("이동 목표 성공 | 충돌한 플레이어 : %s"), *OtherActor->GetName());
        // UJ_Utility::PrintFullLog(debugStr, 3.f, FColor::White);

        // 수행도 1
        SUCCESS_PERCENT = 1.f;
        // 성공 처리
        this->ObjectiveEnd(true);
    }   
}

bool AJ_ObjectiveMovePoint::CheckFail(const FVector &baseDir)
{
    if(!HasAuthority() || IS_OBJ_ENDED) return false;
    
    // 모든 파일럿 중 한 명이라도 기준 방향을 넘어가면 실패 처리
    const auto& allPilot = UJ_Utility::GetAllMissionPawn(GetWorld());
    
    for(const auto* pilot : allPilot)
    {
        // 현재 방향
        const auto& curDir = (this->GetActorLocation() - pilot->GetActorLocation()).GetSafeNormal();

        // 내적
        float check = FVector::DotProduct(baseDir, curDir);
        // 거리
        float dis = FVector::Dist(this->GetActorLocation(), pilot->GetActorLocation());

        // GEngine->AddOnScreenDebugMessage(-1, -1.f, FColor::Green, FString::Printf(TEXT("actor : %s\n내적 중  : %.2f, 거리 : %.2f")
        // , *this->GetName()
        // , check
        // , dis));

        // 실패
        if(check < 0 && dis > failDis)
            return true;
    }

    return false;
}

#pragma endregion