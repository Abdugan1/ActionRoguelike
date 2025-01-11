// Fill out your copyright notice in the Description page of Project Settings.


#include "SEffect_Thorn.h"

#include "SAttributeComponent.h"
#include "SGameplayFunctionLibrary.h"


USEffect_Thorn::USEffect_Thorn()
{
	ReflectCoeff = 0.5f;
}


void USEffect_Thorn::ReflectDamage(AActor* InstigatorActor, USAttributeComponent* OwningComponent, float NewHealth,
	float Delta)
{
	if (Delta < 0.0f)
	{
		const float ReflectDamage = Delta * ReflectCoeff;

		USGameplayFunctionLibrary::ApplyDamage(OwningComponent->GetOwner(), InstigatorActor, ReflectDamage);
	}
}


void USEffect_Thorn::ExecutePeriodEffect_Implementation(AActor* Instigator)
{
	Super::ExecutePeriodEffect_Implementation(Instigator);
}


void USEffect_Thorn::StopAction_Implementation(AActor* Instigator)
{
	Super::StopAction_Implementation(Instigator);

	USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(Instigator);
	if (ensure(AttributeComp))
	{
		AttributeComp->OnHealthChanged.RemoveDynamic(this, &USEffect_Thorn::ReflectDamage);
	}
}


void USEffect_Thorn::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);

	USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(Instigator);
	if (ensure(AttributeComp))
	{
		AttributeComp->OnHealthChanged.AddDynamic(this, &USEffect_Thorn::ReflectDamage);
	}
}
