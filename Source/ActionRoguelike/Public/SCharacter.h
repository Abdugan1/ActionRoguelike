// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SCharacter.generated.h"

class USActionComponent;
class USPlayerAttributeComponent;
class ASProjectileBase;
class UAnimMontage;
class USInteractionComponent;
class UCameraComponent;
class USpringArmComponent;

UCLASS()
class ACTIONROGUELIKE_API ASCharacter : public ACharacter
{
	GENERATED_BODY()
public:
	ASCharacter();

	void PostInitializeComponents() override;

	virtual FVector GetPawnViewLocation() const override;

	UFUNCTION(Exec)
	void HealSelf(float Amount = 200);

protected:
	void MoveForward(float X);
	void MoveRight(float X);

	void SprintStart();
	void SprintStop();

	void PrimaryAttack();
	void BlackholeAttack();
	void DashAttack();

	void PrimaryInteract();

	UFUNCTION()
	void OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComponent, float NewHealth, float Delta);

protected:
	UFUNCTION(Server, Reliable)
	void ServerApplyRageChange(AActor *InstigatorActor, float Delta);

public:	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	UPROPERTY(VisibleAnywhere, Category="Components")
	USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere, Category="Components")
	UCameraComponent* CameraComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USInteractionComponent* InteractionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USPlayerAttributeComponent* AttributeComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USActionComponent* ActionComp;

	UPROPERTY(VisibleAnywhere, Category = "Params|Effects")
	FName HitFlashParamName;	
};
