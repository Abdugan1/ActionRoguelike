// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SPowerupActor.h"
#include "SPowerup_GrantDash.generated.h"

class USAction;
/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API ASPowerup_GrantDash : public ASPowerupActor
{
	GENERATED_BODY()

public:
	virtual void Interact_Implementation(APawn* InstigatorPawn) override;
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Powerup")
	TSubclassOf<USAction> DashAction;
};
