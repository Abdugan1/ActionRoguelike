// Fill out your copyright notice in the Description page of Project Settings.


#include "SDashProjectile.h"


#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"


ASDashProjectile::ASDashProjectile()
{
	DetonateDelay = 0.2f;
	TeleportDelay = 0.2f;

	MovementComp->InitialSpeed = 6000.0f;
}


void ASDashProjectile::BeginPlay()
{
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(
		TimerHandle_DelayedDetonate, 
		this, 
		&ASDashProjectile::Explode, 
		DetonateDelay
	);
}

void ASDashProjectile::Explode_Implementation()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_DelayedDetonate);

	UGameplayStatics::SpawnEmitterAtLocation(this, ImpactVFX, GetActorLocation(), GetActorRotation());

	EffectComp->DeactivateSystem();

	MovementComp->StopMovementImmediately();
	SetActorEnableCollision(false);

	FTimerHandle TimerHandle_DelayedTeleport;
	GetWorldTimerManager().SetTimer(
		TimerHandle_DelayedTeleport, 
		this,
		&ASDashProjectile::TeleportInstigator, 
		TeleportDelay
	);

	// Do not call this. We need the projectile to teleport the instigator. Only after teleporting the instigator can we 
	//Super::Explode_Implementation();
}


void ASDashProjectile::TeleportInstigator()
{
	auto ActorToTeleport = GetInstigator();
	if (ensure(ActorToTeleport))
	{
		ActorToTeleport->TeleportTo(GetActorLocation(), ActorToTeleport->GetActorRotation(), false, false);
	}

	Destroy();
}

