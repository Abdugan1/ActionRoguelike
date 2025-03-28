// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameplayFunctionLibrary.h"

#include "SAttributeComponent.h"


bool USGameplayFunctionLibrary::ApplyDamage(AActor* DamageCauser, AActor* TargetActor, float DamageAmount)
{
	USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(TargetActor);
	if (AttributeComp)
	{
		const float Damage = (DamageAmount < 0.0f ? DamageAmount : -DamageAmount);
		return AttributeComp->ApplyHealthChange(DamageCauser, Damage);
	}
	return false;
}


bool USGameplayFunctionLibrary::ApplyDirectionalDamage(AActor* DamageCauser, AActor* TargetActor, float DamageAmount,
	const FHitResult& HitResult)
{
	if (ApplyDamage(DamageCauser, TargetActor, DamageAmount))
	{
		UPrimitiveComponent* HitComp = HitResult.GetComponent();
		if (HitComp && HitComp->IsSimulatingPhysics(HitResult.BoneName))
		{
			// Direction = Target - Origin
			FVector Direction = HitResult.TraceEnd - HitResult.TraceStart;
			Direction.Normalize();

			HitComp->AddImpulseAtLocation(Direction * 3000.0f, HitResult.ImpactPoint, HitResult.BoneName);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("The Hit Actor: '%s' and bone '%s' does not simulate physics. Skipping apply impulse"), *GetNameSafe(HitComp), *HitResult.BoneName.ToString());
		}
		return true;
	}
	return false;
}
