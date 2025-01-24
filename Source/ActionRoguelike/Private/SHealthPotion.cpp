// Fill out your copyright notice in the Description page of Project Settings.


#include "SHealthPotion.h"

#include "SAttributeComponent.h"
#include "SPlayerState.h"



ASHealthPotion::ASHealthPotion()
{
	HealAmount = 50.0f;

	CreditCost = 20;
}


void ASHealthPotion::Interact_Implementation(APawn* InstigatorPawn)
{
	if (ensure(InstigatorPawn))
	{
		ASPlayerState* PlayerState = ASPlayerState::GetPlayerStateOfPawn(InstigatorPawn);
		if (!ensure(PlayerState))
		{
			return;
		}

		const int32 PlayerCredits = PlayerState->GetCredits();

		const bool bIsEnoughCredits = PlayerCredits >= CreditCost;

		if (!bIsEnoughCredits)
		{
			UE_LOG(LogTemp, Log, TEXT("Not enough credits to use potion. Cost: %i"), CreditCost);
			return;
		}

		auto AttributeComponent = USAttributeComponent::GetAttributes(InstigatorPawn);

		if (ensure(AttributeComponent) && !AttributeComponent->IsFullHealth())
		{
			if (AttributeComponent->ApplyHealthChange(this, HealAmount))
			{
				HideAndCooldownPowerup();
				PlayerState->ApplyCreditsChange(-CreditCost);
			}
		}
	}
	ISGameplayInterface::Interact_Implementation(InstigatorPawn);
}


FText ASHealthPotion::GetInteractText_Implementation(APawn* InstigatorPawn)
{
	auto AttributeComponent = USAttributeComponent::GetAttributes(InstigatorPawn);
	if (AttributeComponent && AttributeComponent->IsFullHealth())
	{
		return NSLOCTEXT("InteractableActors", "HealthPotion_FullHealthWarning", "Already at full health");
	}

	return FText::Format(NSLOCTEXT("InteractableActors", "HealthPotion_InteractMessage", "Cost {0} Credits. Restores {1} amount of health."), CreditCost, HealAmount);
}
