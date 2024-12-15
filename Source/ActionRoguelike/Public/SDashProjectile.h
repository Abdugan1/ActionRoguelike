// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SProjectileBase.h"
#include "GameFramework/Actor.h"
#include "Particles/ParticleSystem.h"
#include "SDashProjectile.generated.h"


UCLASS()
class ACTIONROGUELIKE_API ASDashProjectile : public ASProjectileBase

{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASDashProjectile();


protected:
	virtual void BeginPlay() override;

	virtual void Explode_Implementation() override;

	void TeleportInstigator();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UParticleSystem* ExplodeEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Teleport")
	float DetonateDelay;

	UPROPERTY(EditDefaultsOnly, Category = "Teleport")
	float TeleportDelay;

protected:

	FTimerHandle TimerHandle_DelayedDetonate;

};
