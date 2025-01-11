// Fill out your copyright notice in the Description page of Project Settings.


#include "SPowerup_GrantDash.h"

#include "SActionComponent.h"


void ASPowerup_GrantDash::Interact_Implementation(APawn* InstigatorPawn)
{
	Super::Interact_Implementation(InstigatorPawn);

	USActionComponent *ActionComp = USActionComponent::GetActionComponent(InstigatorPawn);
	if (ensure(ActionComp))
	{
		if (ActionComp->HasAction(DashAction))
		{
			return;
		}

		ActionComp->AddAction(this, DashAction);
		HideAndCooldownPowerup();
	}
}
