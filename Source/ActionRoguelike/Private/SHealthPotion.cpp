// Fill out your copyright notice in the Description page of Project Settings.


#include "SHealthPotion.h"

#include "SAttributeComponent.h"


ASHealthPotion::ASHealthPotion()
{
	HealAmount = 50.0f;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Mesh->SetupAttachment(RootComponent);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}


void ASHealthPotion::Interact_Implementation(APawn* InstigatorPawn)
{
	if (ensure(InstigatorPawn))
	{
		auto AttributeComponent = Cast<USAttributeComponent>(
			InstigatorPawn->GetComponentByClass(USAttributeComponent::StaticClass())
		);

		if (ensure(AttributeComponent) && !AttributeComponent->IsFullHealth())
		{
			if (AttributeComponent->ApplyHealthChange(HealAmount))
				HideAndCooldownPowerup();
		}
	}
	ISGameplayInterface::Interact_Implementation(InstigatorPawn);
}