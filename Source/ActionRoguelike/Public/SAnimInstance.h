// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "SAnimInstance.generated.h"

class USActionComponent;
/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API USAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
protected:
	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	/* Is Pawn Stunned based on GameplayTags data */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	bool bIsStunned;

	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	USActionComponent* ActionComp;
};
