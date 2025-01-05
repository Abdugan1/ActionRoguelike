// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "SPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCreditsChanged, float, NewCredits, float, Delta);

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API ASPlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	ASPlayerState();

	float GetCredits() const;
	void ApplyCreditsChange(float Delta);

	UPROPERTY(BlueprintAssignable)
	FOnCreditsChanged OnCreditsChanged;

	static ASPlayerState* GetPlayerStateOfPawn(APawn* Pawn);

protected:
	UPROPERTY(VisibleAnywhere, Category = "PlayerState")
	float Credits;
};
