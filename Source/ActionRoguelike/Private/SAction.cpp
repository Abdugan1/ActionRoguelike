// Fill out your copyright notice in the Description page of Project Settings.


#include "SAction.h"
#include "SActionComponent.h"
#include "ActionRoguelike/ActionRoguelike.h"
#include "Net/UnrealNetwork.h"


static TAutoConsoleVariable<bool> CVarActionsDebug{ TEXT("su.ActionsDebug"), false, TEXT("Show actions started/stopped on screen"), ECVF_Cheat };


void USAction::StartAction_Implementation(AActor* Instigator)
{
	//UE_LOG(LogTemp, Log, TEXT("Running: %s"), *GetNameSafe(this));
	const bool bDebug = CVarActionsDebug.GetValueOnGameThread();
	if (bDebug)
	{
		LogOnScreen(this, FString::Printf(TEXT("Running: %s"), *GetNameSafe(this)), FColor::Green);
	}

	USActionComponent* Comp = GetOwningComponent();
	Comp->ActiveGameplayTags.AppendTags(GrantsTags);

	RepData.bIsRunning = true;
	RepData.Instigator = Instigator;

	// Is Server?
	if (Comp->GetOwnerRole() == ROLE_Authority)
	{
		TimeStarted = GetWorld()->TimeSeconds;
	}

	Comp->OnActionStarted.Broadcast(Comp, this);
}


void USAction::StopAction_Implementation(AActor* Instigator)
{
	//UE_LOG(LogTemp, Log, TEXT("Stopped: %s"), *GetNameSafe(this));
	const bool bDebug = CVarActionsDebug.GetValueOnGameThread();
	if (bDebug)
	{
		LogOnScreen(this, FString::Printf(TEXT("Stopped: %s"), *GetNameSafe(this)), FColor::White);
	}

	// No longer a valid sanity check, as it's only valid in the server.
	//ensureAlways(bIsRunning);

	USActionComponent* Comp = GetOwningComponent();
	Comp->ActiveGameplayTags.RemoveTags(GrantsTags);

	RepData.bIsRunning = false;
	RepData.Instigator = Instigator;

	Comp->OnActionStopped.Broadcast(Comp, this);
}


void USAction::OnRep_RepData()
{
	if (RepData.bIsRunning)
	{
		StartAction(RepData.Instigator);
	}
	else
	{
		StopAction(RepData.Instigator);
	}
}


class UWorld* USAction::GetWorld() const
{
	// Outer is set when creating action via NewObject<T>
	UActorComponent *Comp = Cast<UActorComponent>(GetOuter());
	if (Comp)
	{
		return Comp->GetWorld();
	}
	return nullptr;
}


bool USAction::CanStart_Implementation(AActor* InstigatorActor) const
{
	if (IsRunning())
	{
		return false;
	}
	USActionComponent* Comp = GetOwningComponent();
	if (Comp->ActiveGameplayTags.HasAny(BlockedTags))
	{
		return false;
	}
	return true;
}


bool USAction::IsRunning() const
{
	return RepData.bIsRunning;
}


USActionComponent* USAction::GetOwningComponent() const
{
	return Cast<USActionComponent>(GetOuter());
}


void USAction::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USAction, TimeStarted);
	DOREPLIFETIME(USAction, RepData);
}
