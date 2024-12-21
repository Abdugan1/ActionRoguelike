// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SCharacter.generated.h"

class USAttributeComponent;
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
	// Sets default values for this character's properties
	ASCharacter();

	virtual void PostInitProperties() override;

protected:
	void MoveForward(float X);
	void MoveRight(float X);

	void PrimaryAttack();
	void BlackholeAttack();
	void DashAttack();

	void Attack(const TSubclassOf<AActor> &ProjectileClass);
	void AttackElapsedTime(const TSubclassOf<AActor> &ProjectileClass);

	void PrimaryInteract();

	UFUNCTION()
	void OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComponent, float NewHealth, float Delta);

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
	USAttributeComponent* AttributeComponent;

protected:
	UPROPERTY(EditAnywhere, Category = "Attack")
	TSubclassOf<AActor> PrimaryProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Attack")
	TSubclassOf<AActor> BlackholeProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Attack")
	TSubclassOf<AActor> DashProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Attack")
	UParticleSystem* CastSpellEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	float CrosshairAttackLineSweepLenght;

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	float CrosshairAttackLineSweepShapeRadius;

	UPROPERTY(EditAnywhere, Category = "Animation")
	UAnimMontage* AttackAnim;

	FTimerHandle TimerHandlePrimaryAttack;

};
