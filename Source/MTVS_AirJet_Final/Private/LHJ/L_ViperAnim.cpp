// Fill out your copyright notice in the Description page of Project Settings.


#include "LHJ/L_ViperAnim.h"

#include "LHJ/L_Viper.h"

void UL_ViperAnim::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	Viper = Cast<AL_Viper>(TryGetPawnOwner());
}

void UL_ViperAnim::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if (!Viper)
		return;

	Pitch = Viper->CanopyPitch;
	FrontWheel = Viper->FrontWheel;
	RearLWheel = Viper->RearLWheel;
	RearRWheel = Viper->RearRWheel;
}
