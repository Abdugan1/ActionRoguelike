// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "SPlayerState.generated.h"

class USSaveGame;
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

	UFUNCTION(BlueprintCallable, Category = "PlayerState")
	int32 GetCredits() const;

	UFUNCTION(BlueprintCallable, Category = "PlayerState")
	void ApplyCreditsChange(float Delta);

	UPROPERTY(BlueprintAssignable)
	FOnCreditsChanged OnCreditsChanged;

	static ASPlayerState* GetPlayerStateOfPawn(APawn* Pawn);

	UFUNCTION(BlueprintNativeEvent)
	void SavePlayerState(USSaveGame* SaveObject);

	UFUNCTION(BlueprintNativeEvent)
	void LoadPlayerState(USSaveGame* SaveObject);

protected:
	UFUNCTION(NetMulticast, Reliable)
	void MulticastOnCreditsChanged(float NewCredits, float Delta);

protected:
	UPROPERTY(VisibleAnywhere, Replicated, Category = "PlayerState")
	int32 Credits;
};
