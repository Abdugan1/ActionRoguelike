// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerAttributeComponent.h"

#include "Net/UnrealNetwork.h"


USPlayerAttributeComponent::USPlayerAttributeComponent()
{
	Rage = 0.0f;
	RageMax = 100;

	SetIsReplicatedByDefault(true);
}


bool USPlayerAttributeComponent::ApplyRageChange(AActor* InstigatorActor, float Delta) // @todo Do I need an Instigator here? 
{
	const float OldRage = Rage;
	Rage = FMath::Clamp(Rage + Delta, 0, RageMax);

	const float ActualDelta = Rage - OldRage;
	if (!FMath::IsNearlyZero(ActualDelta))
	{
		//OnRageChanged.Broadcast(InstigatorActor, this, Rage, Delta);
		MulticastRageChanged(InstigatorActor, Rage, ActualDelta);
	}

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


void USPlayerAttributeComponent::MulticastRageChanged_Implementation(AActor* InstigatorActor, float NewRage, float Delta)
{
	OnRageChanged.Broadcast(InstigatorActor, this, NewRage, Delta);
}


void USPlayerAttributeComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USPlayerAttributeComponent, Rage);
	DOREPLIFETIME(USPlayerAttributeComponent, RageMax);
}
