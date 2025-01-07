// Fill out your copyright notice in the Description page of Project Settings.


#include "SActionEffect.h"

#include "SActionComponent.h"


USActionEffect::USActionEffect()
{
	bAutoStart = true;
}


void USActionEffect::ExecutePeriodEffect_Implementation(AActor* Instigator)
{
}


void USActionEffect::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);

	if (Duration > 0.0f)
	{
		FTimerDelegate Delegate;
		Delegate.BindUObject(this, &USActionEffect::StopAction, Instigator);

		GetWorld()->GetTimerManager().SetTimer(TimerHandle_Duration, Delegate, Duration, false);
	}

	if (Period > 0.0f)
	{
		FTimerDelegate Delegate;
		Delegate.BindUObject(this, &USActionEffect::ExecutePeriodEffect, Instigator);

		GetWorld()->GetTimerManager().SetTimer(TimerHandle_Period, Delegate, Period, true);
	}
}


void USActionEffect::StopAction_Implementation(AActor* Instigator)
{
	if (GetWorld()->GetTimerManager().GetTimerRemaining(TimerHandle_Period) < KINDA_SMALL_NUMBER)
	{
		ExecutePeriodEffect(Instigator);
	}

	Super::StopAction_Implementation(Instigator);

	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Duration);
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Period);

	USActionComponent *Comp = GetOwningComponent();
	if (Comp)
	{
		Comp->RemoveAction(this);
	}
}
