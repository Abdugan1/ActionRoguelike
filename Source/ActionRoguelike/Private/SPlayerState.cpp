// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerState.h"

#include "SSaveGame.h"
#include "Net/UnrealNetwork.h"


ASPlayerState::ASPlayerState()
{
	Credits = 0;

	bReplicates = true;
}


int32 ASPlayerState::GetCredits() const
{
	return Credits;
}


void ASPlayerState::ApplyCreditsChange(float Delta)
{
	Credits += Delta;
	OnCreditsChanged.Broadcast(this, Credits, Delta);
}


ASPlayerState* ASPlayerState::GetPlayerStateOfPawn(APawn* Pawn)
{
	if (Pawn)
	{
		return Pawn->GetPlayerState<ASPlayerState>();
	}

	return nullptr;
}


void ASPlayerState::OnRep_Credits(int32 OldCredits)
{
	OnCreditsChanged.Broadcast(this, Credits, Credits - OldCredits);
}


void ASPlayerState::SavePlayerState_Implementation(USSaveGame* SaveObject)
{
	if (SaveObject)
	{
		SaveObject->Credits = Credits;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("SaveObject is null! Cannot save game state!"));
	}
}


void ASPlayerState::LoadPlayerState_Implementation(USSaveGame* SaveObject)
{
	if (SaveObject)
	{
		/* We could do this, but it won't trigger the changed event.*/
		//Credits = SaveObject->Credits;
		ApplyCreditsChange(SaveObject->Credits);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("SaveObject is null! Cannot load game state!"));
	}
}


void ASPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASPlayerState, Credits);
}