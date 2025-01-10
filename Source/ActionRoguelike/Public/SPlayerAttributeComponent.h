// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SAttributeComponent.h"
#include "SPlayerAttributeComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnRageChanged, AActor*, InstigatorActor, USPlayerAttributeComponent*, OwningComponent, float, NewRage, float, Delta);

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API USPlayerAttributeComponent : public USAttributeComponent
{
	GENERATED_BODY()
public:
	USPlayerAttributeComponent();

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	bool ApplyRageChange(AActor* InstigatorActor, float Delta);

	UPROPERTY(BlueprintAssignable)
	FOnRageChanged OnRageChanged;

	UFUNCTION(BlueprintCallable)
	float GetMaxRage() const;

	UFUNCTION(BlueprintCallable)
	float GetRageScaled() const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
	float Rage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
	float RageMax;
};
