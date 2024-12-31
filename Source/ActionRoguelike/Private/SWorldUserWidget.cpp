// Fill out your copyright notice in the Description page of Project Settings.


#include "SWorldUserWidget.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/SizeBox.h"
#include "Kismet/GameplayStatics.h"


void USWorldUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!IsValid(AttachedActor))
	{
		RemoveFromParent();
		UE_LOG(LogTemp, Warning, TEXT("AttachedActor no longer valid, removing Widget"));
		return;
	}

	FVector2D ScreenPosition;
	if (UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(
		GetOwningPlayer(), 
		AttachedActor->GetActorLocation() + WorldOffset,
		ScreenPosition,
		true
	))
	{
		if (ParentSizeBox)
		{
			ParentSizeBox->SetRenderTranslation(ScreenPosition);
		}
	}
}
