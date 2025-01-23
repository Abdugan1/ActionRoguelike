// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "SAction.generated.h"

class USActionComponent;


USTRUCT()
struct FActionRepData
{
	GENERATED_BODY()
public:
	UPROPERTY()
	bool bIsRunning;

	UPROPERTY()
	AActor* Instigator;
};


/**
 * 
 */
UCLASS(Blueprintable)
class ACTIONROGUELIKE_API USAction : public UObject
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintNativeEvent, Category = "Action")
	void StartAction(AActor* Instigator);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Action")
	void StopAction(AActor* Instigator);

	UFUNCTION(BlueprintNativeEvent, Category = "Action")
	bool CanStart(AActor *InstigatorActor) const;

	UFUNCTION(BlueprintCallable, Category = "Action")
	bool IsRunning() const;

	UFUNCTION(BlueprintCallable, Category = "Action")
	USActionComponent* GetOwningComponent() const;

	virtual class UWorld* GetWorld() const override;

	virtual bool IsSupportedForNetworking() const override
	{
		return true;
	}

protected:
	UFUNCTION()
	void OnRep_RepData();

public:
	UPROPERTY(EditDefaultsOnly, Category = "Action")
	FName ActionName;

	/* Start immediately when added to an action component */
	UPROPERTY(EditDefaultsOnly, Category = "Action")
	bool bAutoStart;

protected:
	UPROPERTY(ReplicatedUsing="OnRep_RepData")
	FActionRepData RepData;

	/* Tags added to owning actor when activated, removed when action stops */
	UPROPERTY(EditDefaultsOnly, Category = "Tags")
	FGameplayTagContainer GrantsTags;

	/* Action can only start if OwningActor has none of these Tags applied */
	UPROPERTY(EditDefaultsOnly, Category = "Tags")
	FGameplayTagContainer BlockedTags;

	UPROPERTY(Replicated)
	float TimeStarted;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	UTexture2D* Icon;
};
