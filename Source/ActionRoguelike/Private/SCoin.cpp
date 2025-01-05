// Fill out your copyright notice in the Description page of Project Settings.


#include "SCoin.h"

#include "SPlayerState.h"


ASCoin::ASCoin()
{
	GrantCreditsAmount = 20.0f;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Mesh->SetupAttachment(RootComponent);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}


void ASCoin::Interact_Implementation(APawn* InstigatorPawn)
{
	if (ensure(InstigatorPawn))
	{
		ASPlayerState* PlayerState = ASPlayerState::GetPlayerStateOfPawn(InstigatorPawn);
		if (ensure(PlayerState))
		{
			PlayerState->ApplyCreditsChange(GrantCreditsAmount);
			HideAndCooldownPowerup();
		}
	}
}
