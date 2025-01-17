// Fill out your copyright notice in the Description page of Project Settings.


#include "SPowerupActor.h"

#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"


ASPowerupActor::ASPowerupActor()
{
	InteractionSphereComp = CreateDefaultSubobject<USphereComponent>("InteractionSphereComp");
	InteractionSphereComp->SetCollisionProfileName("Powerup");

	RootComponent = InteractionSphereComp;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Mesh->SetupAttachment(RootComponent);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	RespawnTime = 1.0f;

	bPowerupState = true;

	bReplicates = true;
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
	bPowerupState = bIsActive;
	OnRep_PowerupState();
	//SetActorEnableCollision(bIsActive);
	//RootComponent->SetVisibility(bIsActive, true);
}


void ASPowerupActor::OnRep_PowerupState()
{
	SetActorEnableCollision(bPowerupState);
	RootComponent->SetVisibility(bPowerupState, true);
}


void ASPowerupActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASPowerupActor, bPowerupState);
}
