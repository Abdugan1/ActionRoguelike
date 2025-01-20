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
	//OnCreditsChanged.Broadcast(Credits, Delta);
	MulticastOnCreditsChanged(Credits, Delta);
}


ASPlayerState* ASPlayerState::GetPlayerStateOfPawn(APawn* Pawn)
{
	if (Pawn)
	{
		return Pawn->GetPlayerState<ASPlayerState>();
	}

	return nullptr;
}


void ASPlayerState::SavePlayerState_Implementation(USSaveGame* SaveObject)
{
	if (SaveObject)
	{
		SaveObject->Credits = Credits;
	}
}


void ASPlayerState::LoadPlayerState_Implementation(USSaveGame* SaveObject)
{
	if (SaveObject)
	{
		Credits = SaveObject->Credits;
	}
}


void ASPlayerState::MulticastOnCreditsChanged_Implementation(float NewCredits, float Delta)
{
	OnCreditsChanged.Broadcast(NewCredits, Delta);
}


void ASPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASPlayerState, Credits);
}