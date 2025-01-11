// Fill out your copyright notice in the Description page of Project Settings.


#include "SPowerupActor.h"

#include "Components/SphereComponent.h"


ASPowerupActor::ASPowerupActor()
{
	InteractionSphereComp = CreateDefaultSubobject<USphereComponent>("InteractionSphereComp");
	InteractionSphereComp->SetCollisionProfileName("Powerup");

	RespawnTime = 10.0f;

	RootComponent = InteractionSphereComp;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Mesh->SetupAttachment(RootComponent);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}


void ASPowerupActor::Interact_Implementation(APawn* InstigatorPawn)
{
	// Does not need any implementation. Yet it's necessary to have this implementation.
}


void ASPowerupActor::ShowPowerup()
{
	SetPowerupState(true);
}


void ASPowerupActor::HideAndCooldownPowerup()
{
	SetPowerupState(false);

	GetWorldTimerManager().SetTimer(RespawnTimerHandle, this, &ASPowerupActor::ShowPowerup, RespawnTime);
}


void ASPowerupActor::SetPowerupState(bool bIsActive)
{
	SetActorEnableCollision(bIsActive);
	RootComponent->SetVisibility(bIsActive, true);
}
