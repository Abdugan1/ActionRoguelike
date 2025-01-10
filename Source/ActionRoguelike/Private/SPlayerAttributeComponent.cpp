// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerAttributeComponent.h"


USPlayerAttributeComponent::USPlayerAttributeComponent()
{
	Rage = 0.0f;
	RageMax = 100;
}


bool USPlayerAttributeComponent::ApplyRageChange(AActor* InstigatorActor, float Delta) // @todo Do I need an Instigator here? 
{
	const float OldRage = Rage;
	Rage = FMath::Clamp(Rage + Delta, 0, RageMax);

	const float ActualDelta = Rage - OldRage;
	OnRageChanged.Broadcast(InstigatorActor, this, Rage, ActualDelta);

	return !(FMath::IsNearlyZero(ActualDelta));
}


float USPlayerAttributeComponent::GetMaxRage() const
{
	return RageMax;
}


float USPlayerAttributeComponent::GetRageScaled() const
{
	return Rage / RageMax;
}

