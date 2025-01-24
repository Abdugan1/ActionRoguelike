// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SGameplayInterface.h"
#include "GameFramework/Actor.h"
#include "SPowerupActor.generated.h"

class USphereComponent;

UCLASS()
class ACTIONROGUELIKE_API ASPowerupActor : public AActor, public ISGameplayInterface
{
	GENERATED_BODY()
	
public:	
	ASPowerupActor();

	virtual void Interact_Implementation(APawn* InstigatorPawn) override;

	virtual FText GetInteractText_Implementation(APawn* InstigatorPawn) override;

protected:
	void ShowPowerup();
	void HideAndCooldownPowerup();

private:
	void SetPowerupState(bool bIsActive);

	UFUNCTION()
	void OnRep_PowerupState();

protected:
	UPROPERTY(VisibleAnywhere, Category="Components")
	USphereComponent* InteractionSphereComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditDefaultsOnly, Category="Powerup")
	float RespawnTime;

	UPROPERTY(ReplicatedUsing="OnRep_PowerupState")
	bool bPowerupState;

	FTimerHandle RespawnTimerHandle;
};

