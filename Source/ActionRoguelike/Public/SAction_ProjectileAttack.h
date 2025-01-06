// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SAction.h"
#include "SAction_ProjectileAttack.generated.h"

class ASProjectileBase;
/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API USAction_ProjectileAttack : public USAction
{
	GENERATED_BODY()
public:
	USAction_ProjectileAttack();

	virtual void StartAction_Implementation(AActor* Instigator) override;

protected:
	void StartAttackEffects(ACharacter* InstigatorCharacter);
	void AttackDelay_Elapsed(ACharacter* InstigatorCharacter);

protected:
	UPROPERTY(EditAnywhere, Category = "Attack")
	TSubclassOf<ASProjectileBase> ProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Attack|Effects")
	UParticleSystem* CastingEffect;

	UPROPERTY(VisibleAnywhere, Category = "Attack")
	FName HandSocketName;

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	float CrosshairAttackLineSweepLenght;

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	float CrosshairAttackLineSweepShapeRadius;

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	float AttackDelay;

	UPROPERTY(EditAnywhere, Category = "Attack|Animation")

	UAnimMontage* AttackAnim;
};
