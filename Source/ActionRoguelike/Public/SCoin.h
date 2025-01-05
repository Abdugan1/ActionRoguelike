// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SPowerupActor.h"
#include "SCoin.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API ASCoin : public ASPowerupActor
{
	GENERATED_BODY()
public:
	ASCoin();

	virtual void Interact_Implementation(APawn* InstigatorPawn) override;

protected:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditDefaultsOnly, Category = "Coin")
	float GrantCreditsAmount;
};
