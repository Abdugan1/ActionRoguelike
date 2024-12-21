// Fill out your copyright notice in the Description page of Project Settings.


#include "SHealthPotion.h"

#include "SAttributeComponent.h"


ASHealthPotion::ASHealthPotion()
{
	HealAmount = 50.0f;

	RespawnTime = 10.0f;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	RootComponent = Mesh;
}


void ASHealthPotion::Interact_Implementation(APawn* InstigatorPawn)
{
	if (ensure(InstigatorPawn))
	{
		auto AttributeComponent = Cast<USAttributeComponent>(
			InstigatorPawn->GetComponentByClass(USAttributeComponent::StaticClass())
		);

		if (ensure(AttributeComponent))
		{
			if (AttributeComponent->IsFullHealth())
				return;

			AttributeComponent->ApplyHealthChange(HealAmount);
			MakeNonInteractableForTime();
		}
	}
	ISGameplayInterface::Interact_Implementation(InstigatorPawn);
}


bool ASHealthPotion::CanBeInteracted_Implementation() const
{
	return Interactable_;
}


void ASHealthPotion::MakeNonInteractableForTime()
{
	Interactable_ = false;

	SetActorHiddenInGame(true);

	GetWorldTimerManager().SetTimer(
		RespawnTimerHandle, 
		this, 
		&ASHealthPotion::MakeInteractable,
		RespawnTime,
		false
	);
}


void ASHealthPotion::MakeInteractable()
{
	GetWorldTimerManager().ClearTimer(RespawnTimerHandle);

	Interactable_ = true;

	SetActorHiddenInGame(false);
}

