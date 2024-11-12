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
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

	UFUNCTION()
	void DoTeleportationOnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	void DoTeleportation();

	void Explode();
	void TeleportInstigator();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UParticleSystem* ExplodeEffect;

protected:
	FTimerHandle TimerHandleExplode;
	FTimerHandle TimerHandleTeleport;

};
