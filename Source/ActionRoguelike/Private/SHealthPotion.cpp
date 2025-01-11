// Fill out your copyright notice in the Description page of Project Settings.


#include "SHealthPotion.h"

#include "SAttributeComponent.h"
#include "SPlayerState.h"


static TAutoConsoleVariable<float> CVarCreditsHealthPotionCostAmount{ TEXT("su.CreditsHealthPotionCostAmount"), 20, TEXT("Credits health potion cost amount per a use"), ECVF_Cheat };


ASHealthPotion::ASHealthPotion()
{
	HealAmount = 50.0f;
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

		const float PlayerCredits = PlayerState->GetCredits();
		const float CreditCost = CVarCreditsHealthPotionCostAmount.GetValueOnGameThread();

		const bool bIsEnoughCredits = PlayerCredits >= CreditCost || FMath::IsNearlyEqual(PlayerCredits, CreditCost);

		if (!bIsEnoughCredits)
		{
			UE_LOG(LogTemp, Log, TEXT("Not enough credits to use potion. Cost: %f"), CreditCost);
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