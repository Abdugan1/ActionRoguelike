// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SGameplayInterface.h"
#include "GameFramework/Actor.h"
#include "SPotionBase.h"
#include "SHealthPotion.generated.h"

UCLASS()
class ACTIONROGUELIKE_API ASHealthPotion : public ASPotionBase, public ISGameplayInterface
{
	GENERATED_BODY()
	
public:	
	ASHealthPotion();

	virtual void Interact_Implementation(APawn* InstigatorPawn) override;

	virtual bool CanBeInteracted_Implementation() const override;

protected:
	void MakeNonInteractableForTime();

	void MakeInteractable();

protected:
	UPROPERTY(EditDefaultsOnly, Category="Effects")
	float HealAmount;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditDefaultsOnly, Category="Respawn")
	float RespawnTime;

private:
	bool Interactable_ = true;
	FTimerHandle RespawnTimerHandle;
};
