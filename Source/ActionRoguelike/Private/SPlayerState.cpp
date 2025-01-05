// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerState.h"


ASPlayerState::ASPlayerState()
{
	Credits = 0;
}


float ASPlayerState::GetCredits() const
{
	return Credits;
}


void ASPlayerState::ApplyCreditsChange(float Delta)
{
	Credits += Delta;
	OnCreditsChanged.Broadcast(Credits, Delta);
}

ASPlayerState* ASPlayerState::GetPlayerStateOfPawn(APawn* Pawn)
{
	if (Pawn)
	{
		return Pawn->GetPlayerState<ASPlayerState>();
	}

	return nullptr;
}
