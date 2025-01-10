// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SActionEffect.h"
#include "SEffect_Thorn.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API USEffect_Thorn : public USActionEffect
{
	GENERATED_BODY()
public:
	USEffect_Thorn();
protected:
	UFUNCTION()
	void ReflectDamage(AActor* InstigatorActor, USAttributeComponent* OwningComponent, float NewHealth, float Delta);

	virtual void ExecutePeriodEffect_Implementation(AActor* Instigator) override;

public:
	virtual void StartAction_Implementation(AActor* Instigator) override;
	virtual void StopAction_Implementation(AActor* Instigator) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Effect")
	float ReflectCoeff;
};
