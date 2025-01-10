// Fill out your copyright notice in the Description page of Project Settings.


#include "SInteractionComponent.h"

#include "SGameplayInterface.h"
#include "Blueprint/UserWidget.h"
#include "SWorldUserWidget.h"


static TAutoConsoleVariable<bool> CVarDebugDrawInteraction(TEXT("su.InteractionDebugDraw"), false, TEXT("Enable Debug Lines for Interaction Component"), ECVF_Cheat);


USInteractionComponent::USInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	SweepRadius = 20.0f;
	SweepDistance = 500.0f;
	SweepCollisionChannel = ECC_WorldDynamic;
}


void USInteractionComponent::PrimaryInteract() const
{
	if (FocusedActor)
	{
		APawn* MyPawn = Cast<APawn>(GetOwner());
		ISGameplayInterface::Execute_Interact(FocusedActor, MyPawn);
	}
}


void USInteractionComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FindBestInteractable();
}


void USInteractionComponent::FindBestInteractable()
{
	const bool bDebugDraw = CVarDebugDrawInteraction.GetValueOnGameThread();

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(SweepCollisionChannel);

	AActor* MyOwner = GetOwner();

	FVector LineStart;
	FRotator Rotation;
	MyOwner->GetActorEyesViewPoint(LineStart, Rotation);

	const FVector LineEnd = LineStart + (Rotation.Vector() * SweepDistance);

	FCollisionShape Shape;
	Shape.SetSphere(SweepRadius);

	TArray<FHitResult> Hits;
	GetWorld()->SweepMultiByObjectType(Hits, LineStart, LineEnd, FQuat::Identity, ObjectQueryParams, Shape);

	if (bDebugDraw)
	{
		DrawDebugLine(GetWorld(), LineStart, LineEnd, FColor::Emerald, false, 2.0f, 0, 2.0f);
	}

	FocusedActor = nullptr;

	for (auto Hit : Hits)
	{
		if (AActor* HitActor = Hit.GetActor())
		{
			if (HitActor->Implements<USGameplayInterface>())
			{
				//bool bCanBeInteracted = ISGameplayInterface::Execute_CanBeInteracted(HitActor);
				if (bDebugDraw)
				{
					DrawDebugSphere(GetWorld(), Hit.ImpactPoint, SweepRadius, 32, FColor::Green, false, 2.0f);
				}
				FocusedActor = HitActor;
				break;
			}
		}
	}

	if (FocusedActor)
	{
		if (DefaultWidgetInstance == nullptr && ensure(DefaultWidgetClass))
		{
			DefaultWidgetInstance = CreateWidget<USWorldUserWidget>(GetWorld(), DefaultWidgetClass);
		}
		if (DefaultWidgetInstance)
		{
			DefaultWidgetInstance->AttachedActor = FocusedActor;

			if (!DefaultWidgetInstance->IsInViewport())
			{
				DefaultWidgetInstance->AddToViewport();
			}
		}
	}
	else
	{
		if (DefaultWidgetInstance)
		{
			DefaultWidgetInstance->RemoveFromParent();
		}
	}
}
