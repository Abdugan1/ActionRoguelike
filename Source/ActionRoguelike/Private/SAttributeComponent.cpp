// Fill out your copyright notice in the Description page of Project Settings.


#include "SAttributeComponent.h"


USAttributeComponent::USAttributeComponent()
{
	HealthMax = 200;
	Health = HealthMax;
}


bool USAttributeComponent::ApplyHealthChange(float Delta)
{
	float OldHealth = Health;
	Health = FMath::Clamp(Health + Delta, 0, HealthMax);

	float ActualDelta = Health - OldHealth;
	OnHealthChanged.Broadcast(nullptr, this, Health, ActualDelta); // @fixme No instigator is on health changed.

	return !(FMath::IsNearlyZero(ActualDelta));
}


bool USAttributeComponent::IsAlive() const
{
	return Health > 0.0f;
}


bool USAttributeComponent::IsFullHealth() const
{
	return Health >= HealthMax && FMath::IsNearlyEqual(Health, HealthMax);
}


float USAttributeComponent::GetHealth() const
{
	return Health;
}


float USAttributeComponent::GetMaxHealth() const
{
	return HealthMax;
}


float USAttributeComponent::GetHealthScaled() const
{
	return (Health / HealthMax);
}

