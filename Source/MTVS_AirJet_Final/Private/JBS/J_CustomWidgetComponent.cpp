// Fill out your copyright notice in the Description page of Project Settings.


#include "JBS/J_CustomWidgetComponent.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "Math/MathFwd.h"

void UJ_CustomWidgetComponent::BeginPlay()
{
    Super::BeginPlay();

    // 로컬 pc 구하기
    auto* tempPC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    check(tempPC);
    localPC = tempPC;

    // 원래 스케일 저장
    // orgUIScale = this->GetRelativeScale3D();
    orgUIScale = FVector::OneVector;
    // FIXME
    this->SetActive(false);
	this->SetHiddenInGame(!false);

}

void UJ_CustomWidgetComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // 로컬 에서 실행
    if(!localPC) return;
    
    if(localPC->IsLocalController() || localPC->HasAuthority() && !localPC->IsLocalController())
    {
        if(enableBillboard)
            Billboard(localPC);

        if(enableFixScale)
            FixScale(targetActor);

        if(enableFixHeight)
            FixHeight(localPC);
    }



    
}


void UJ_CustomWidgetComponent::Billboard(APlayerController * pc)
{
    if(!pc) return;
    // GEngine->AddOnScreenDebugMessage(-1, -1.f, FColor::Green, FString::Printf(TEXT("awodijwad: %s"), *pc->GetName()));
    FVector targetLoc = pc->PlayerCameraManager->GetCameraLocation();
    FVector worldUILoc = this->GetComponentLocation();
    
    FRotator lookRot = (targetLoc - worldUILoc).Rotation();
    this->SetWorldRotation(lookRot);
}

void UJ_CustomWidgetComponent::FixScale(AActor *target)
{
    if(!target) return;

    FixScale(target->GetActorLocation());
}

void UJ_CustomWidgetComponent::FixScale(FVector targetLocation)
{   
    // 크기 일정하게 조정
    // 캠과의 거리 구하기
    float toCamDis = FVector::Dist(targetLocation, this->GetComponentLocation());
    // 크기 구하기
    float newScale = toCamDis / baseDistance;
    // 크기 적용
    this->SetWorldScale3D(FVector(newScale) * orgUIScale);
}

void UJ_CustomWidgetComponent::FixHeight(APlayerController * pc)
{
    if(!pc) return;

    float height = pc->PlayerCameraManager->GetCameraLocation().Z;

    FVector temp = this->GetComponentLocation();
    temp.Z = height;

    this->SetWorldLocation(temp);
}

void UJ_CustomWidgetComponent::SetVisible(bool value)
{
    this->SetActive(value);
	this->SetHiddenInGame(!value);
}

void UJ_CustomWidgetComponent::MRPC_SetVisible(bool value)
{
    SetVisible(value);
}