// Fill out your copyright notice in the Description page of Project Settings.


#include "SActionComponent.h"


#include "SAction.h"
#include "SAttributeComponent.h"
#include "ActionRoguelike/ActionRoguelike.h"
#include "Engine/ActorChannel.h"
#include "Net/UnrealNetwork.h"


static TAutoConsoleVariable<bool> CVarActionCompDebug{ TEXT("su.ActionCompDebug"), false, TEXT("Show action component stats"), ECVF_Cheat };


USActionComponent::USActionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);
}


void USActionComponent::BeginPlay()
{
	Super::BeginPlay();

	// Server Only
	if (GetOwner()->HasAuthority())
	{
		for (TSubclassOf<USAction> ActionClass : DefaultActions)
		{
			AddAction(GetOwner(), ActionClass);
		}
	}
}


void USActionComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//FString DebugMsg = GetNameSafe(GetOwner()) + " : " + ActiveGameplayTags.ToStringSimple();
	//GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::White, DebugMsg);

	const bool bDebug = CVarActionCompDebug.GetValueOnGameThread();
	if (!bDebug)
	{
		return;
	}

	for (USAction *Action : Actions)
	{
		FColor TextColor = Action->IsRunning() ? FColor::Blue : FColor::White;

		FString ActionMsg = FString::Printf(TEXT("[%s] Action: %s"),
			*GetNameSafe(GetOwner()),
			*GetNameSafe(Action)
		);

		LogOnScreen(this, ActionMsg, TextColor, 0.0f);
	}
}


void USActionComponent::AddAction(AActor* Instigator, TSubclassOf<USAction> ActionClass)
{
	if (!ensure(ActionClass))
	{
		return;
	}

	// Skip for clients
	if (!GetOwner()->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("Client attempting to AddAction. [Class: %s]"), *GetNameSafe(ActionClass));
		return;
	}

	USAction* NewAction = NewObject<USAction>(this, ActionClass);
	if (ensure(NewAction))
	{
		Actions.Add(NewAction);

		if (NewAction->bAutoStart && ensure(NewAction->CanStart(Instigator)))
		{
			NewAction->StartAction(Instigator);
		}

		FString DebugMsg = FString::Printf(TEXT("Action: [%s] was added"), *NewAction->GetName());
		LogOnScreen(this, DebugMsg);
	}
}


void USActionComponent::RemoveAction(USAction* Action)
{
	if (!ensure(Action && !Action->IsRunning()))
	{
		return;
	}

	Actions.Remove(Action);

	FString DebugMsg = FString::Printf(TEXT("Action: [%s] was removed"), *Action->GetName());
	LogOnScreen(this, DebugMsg);
}


bool USActionComponent::HasAction(TSubclassOf<USAction> Action) const
{
	ensure(Action);

	for (const USAction *ExistingAction : Actions)
	{
		if (ExistingAction && ExistingAction->GetClass() == Action)
		{
			return true;
		}
	}

	return false;
}


bool USActionComponent::StartActionByName(AActor* Instigator, FName ActionName)
{
	for (USAction *Action : Actions)
	{
		if (Action && Action->ActionName == ActionName)
		{
			if (!Action->CanStart(Instigator))
			{
				FString FailedMsg = FString::Printf(TEXT("Failed to run %s"), *ActionName.ToString());
				GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FailedMsg);
				continue;
			}

			// Is Client?
			if (!GetOwner()->HasAuthority())
			{
				ServerStartAction(Instigator, ActionName);
			}

			Action->StartAction(Instigator);
			return true;
		}
	}
	return false;
}


bool USActionComponent::StopActionByName(AActor* Instigator, FName ActionName)
{
	for (USAction* Action : Actions)
	{
		if (Action && Action->ActionName == ActionName)
		{
			if (Action->IsRunning())
			{
				// Is Client?
				if (!GetOwner()->HasAuthority())
				{
					ServerStopAction(Instigator, ActionName);
				}

				Action->StopAction(Instigator);

				return true;
			}
		}
	}
	return false;
}

USActionComponent* USActionComponent::GetActionComponent(AActor* FromActor)
{
	if (FromActor)
	{
		return Cast<USActionComponent>(FromActor->GetComponentByClass(USActionComponent::StaticClass()));
	}
	return nullptr;
}


void USActionComponent::ServerStartAction_Implementation(AActor* Instigator, FName ActionName)
{
	StartActionByName(Instigator, ActionName);
}


void USActionComponent::ServerStopAction_Implementation(AActor* Instigator, FName ActionName)
{
	StopActionByName(Instigator, ActionName);
}


void USActionComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USActionComponent, Actions);
}


bool USActionComponent::ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch,
	FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for (USAction *Action : Actions)
	{
		if (Action)
		{
			WroteSomething |= Channel->ReplicateSubobject(Action, *Bunch, *RepFlags);
		}
	}

	return WroteSomething;
}

