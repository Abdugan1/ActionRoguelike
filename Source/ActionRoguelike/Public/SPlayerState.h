// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "SPlayerState.generated.h"

class USSaveGame;
class ASPlayerState;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnCreditsChanged, ASPlayerState*, PlayerState, float, NewCredits, float, Delta);

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
	// Downside of using this is that we send over more data over the net, since it's an RPC with two parameters. OnRep_ is "free"
	// since Credits is already getting replicated anyway
	//UFUNCTION(NetMulticast, Reliable)
	//void MulticastOnCreditsChanged(float NewCredits, float Delta);

	UFUNCTION()
	void OnRep_Credits(int32 OldCredits);

protected:
	UPROPERTY(VisibleAnywhere, ReplicatedUsing="OnRep_Credits", Category = "PlayerState")
	int32 Credits;
};
