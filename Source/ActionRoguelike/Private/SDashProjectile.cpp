// Fill out your copyright notice in the Description page of Project Settings.


#include "SDashProjectile.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ASDashProjectile::ASDashProjectile()
{
}

// Called when the game starts or when spawned
void ASDashProjectile::BeginPlay()
{
	Super::BeginPlay();
	GetWorldTimerManager().SetTimer(TimerHandleExplode, this, &ASDashProjectile::DoTeleportation, 0.2f, false);
}

void ASDashProjectile::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	SphereComp->OnComponentHit.AddDynamic(this, &ASDashProjectile::DoTeleportationOnHit);
}

void ASDashProjectile::DoTeleportationOnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	GetWorldTimerManager().ClearTimer(TimerHandleExplode);
	DoTeleportation();
}

void ASDashProjectile::DoTeleportation()
{
	Explode();
	GetWorldTimerManager().SetTimer(TimerHandleTeleport, this, &ASDashProjectile::TeleportInstigator, 0.2f, false);
	TeleportInstigator();
	Destroy();
}

void ASDashProjectile::Explode()
{
	MovementComp->StopMovementImmediately();
	EffectComp->SetVisibility(false);
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplodeEffect, GetActorLocation(), GetActorRotation());
}

void ASDashProjectile::TeleportInstigator()
{
	FVector Location = GetActorLocation();
	FRotator Rotation = GetActorRotation();

	bool SuccessTeleport = GetInstigator()->TeleportTo(Location, Rotation);
	UE_LOG(LogTemp, Warning, TEXT("Success: %s"), (SuccessTeleport ? TEXT("True") : TEXT("False")));
}

