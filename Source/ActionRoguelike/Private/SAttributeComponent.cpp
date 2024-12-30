// Fill out your copyright notice in the Description page of Project Settings.


#include "SAttributeComponent.h"


USAttributeComponent::USAttributeComponent()
{
	HealthMax = 200;
	Health = HealthMax;
}


USAttributeComponent* USAttributeComponent::GetAttributes(AActor* FromActor)
{
	if (FromActor)
	{
		/* UE 5 Version. Casts FOR YOU */
		return FromActor->FindComponentByClass<USAttributeComponent>();
		/* UE 4 Version. HAVE TO Cast MANUALLY */
		//return Cast<USAttributeComponent>(FromActor->GetComponentByClass(USAttributeComponent::StaticClass()));
	}

	return nullptr;
}


bool USAttributeComponent::IsActorAlive(AActor* Actor)
{
	USAttributeComponent* AttributeComp = GetAttributes(Actor);
	if (AttributeComp)
	{
		return AttributeComp->IsAlive();
	}
	return false;
}


bool USAttributeComponent::ApplyHealthChange(AActor* InstigatorActor, float Delta)
{
	float OldHealth = Health;
	Health = FMath::Clamp(Health + Delta, 0, HealthMax);

	float ActualDelta = Health - OldHealth;
	OnHealthChanged.Broadcast(InstigatorActor, this, Health, ActualDelta);

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

