// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerController.h"

#include "Blueprint/UserWidget.h"


void ASPlayerController::BeginPlayingState()
{
	Super::BeginPlayingState();

	BlueprintBeginPlayingState();
}


void ASPlayerController::TogglePauseMenu()
{
	if (PauseMenInstance && PauseMenInstance->IsInViewport())
	{
		PauseMenInstance->RemoveFromParent();
		PauseMenInstance = nullptr;

		bShowMouseCursor = false;
		SetInputMode(FInputModeGameOnly());
		return;
	}

	PauseMenInstance = CreateWidget<UUserWidget>(this, PauseMenuClass);
	if (PauseMenInstance)
	{
		PauseMenInstance->AddToViewport(100);

		bShowMouseCursor = true;
		SetInputMode(FInputModeUIOnly());
	}
}


void ASPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("PauseMenu", IE_Pressed, this, &ASPlayerController::TogglePauseMenu);
}
