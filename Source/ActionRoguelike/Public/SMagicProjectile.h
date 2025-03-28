// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "SProjectileBase.h"
#include "GameFramework/Actor.h"
#include "SMagicProjectile.generated.h"

class USActionEffect;

UCLASS()
class ACTIONROGUELIKE_API ASMagicProjectile : public ASProjectileBase
{
	GENERATED_BODY()
	
public:	
	ASMagicProjectile();

protected:
	virtual void PostInitializeComponents() override;

	UFUNCTION()
	void OnActorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	UPROPERTY(EditDefaultsOnly, Category="Attack")
	float DamageAmount;

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	FGameplayTag ParryTag;

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	TSubclassOf<USActionEffect> BurningActionClass;
};
