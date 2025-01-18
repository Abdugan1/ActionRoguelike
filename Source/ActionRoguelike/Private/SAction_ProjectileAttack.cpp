// Fill out your copyright notice in the Description page of Project Settings.


#include "SAction_ProjectileAttack.h"

#include <Kismet/GameplayStatics.h>
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"

#include "SProjectileBase.h"


static TAutoConsoleVariable<bool> CVarDebugDrawAttackSweep(TEXT("su.AttackSweepDebugDraw"), false, TEXT("Enable Debug Lines for Attack Sweep"), ECVF_Cheat);


USAction_ProjectileAttack::USAction_ProjectileAttack()
{
	CrosshairAttackLineSweepLenght = 5000;
	CrosshairAttackLineSweepShapeRadius = 20;

	AttackDelay = 0.2f;

	HandSocketName = "Muzzle_01";
}


void USAction_ProjectileAttack::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);

	ACharacter* Character = Cast<ACharacter>(Instigator);

	if (Character)
	{
		StartAttackEffects(Character);

		// Is Server?
		if (Character->HasAuthority())
		{
			FTimerHandle TimerHandle_AttackDelay;

			FTimerDelegate Delegate;
			Delegate.BindUObject(this, &USAction_ProjectileAttack::AttackDelay_Elapsed, Character);

			GetWorld()->GetTimerManager().SetTimer(TimerHandle_AttackDelay, Delegate, AttackDelay, false);
		}
	}
}


void USAction_ProjectileAttack::StartAttackEffects(ACharacter* InstigatorCharacter)
{
	InstigatorCharacter->PlayAnimMontage(AttackAnim);

	UGameplayStatics::SpawnEmitterAttached(
		CastingEffect,
		InstigatorCharacter->GetMesh(),
		HandSocketName
	);
}


void USAction_ProjectileAttack::AttackDelay_Elapsed(ACharacter* InstigatorCharacter)
{
	if (ensureAlways(ProjectileClass))
	{
		const FVector HandLocation = InstigatorCharacter->GetMesh()->GetSocketLocation(HandSocketName);

		const FVector ViewLocation = InstigatorCharacter->GetPawnViewLocation();
		const FVector ForwardDireciton = InstigatorCharacter->GetControlRotation().Vector();

		const FVector TraceStart = ViewLocation + ForwardDireciton * CrosshairAttackLineSweepShapeRadius;
		FVector TraceEnd = TraceStart + (ForwardDireciton * CrosshairAttackLineSweepLenght);

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.Instigator = InstigatorCharacter;

		FCollisionShape Shape;
		Shape.SetSphere(CrosshairAttackLineSweepShapeRadius);

		FCollisionQueryParams Params;
		Params.AddIgnoredActor(InstigatorCharacter);

		FCollisionObjectQueryParams ObjectQueryParams;
		ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
		ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
		ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);
		ObjectQueryParams.AddObjectTypesToQuery(ECC_PhysicsBody); // @todo Tom does NOT have this one. Maybe this is the issue


		FHitResult Hit;
		const bool HitSomething = GetWorld()->SweepSingleByObjectType(Hit, TraceStart, TraceEnd, FQuat::Identity, ObjectQueryParams, Shape, Params);

		const bool bDrawDebug = CVarDebugDrawAttackSweep.GetValueOnGameThread();
		const float DrawDebugLifeSpawn = 5.0f;

		if (HitSomething)
		{
			TraceEnd = Hit.ImpactPoint;
		}

		if (bDrawDebug)
		{
			DrawDebugString(GetWorld(), TraceStart, TEXT("START"), nullptr, FColor::Cyan, DrawDebugLifeSpawn);
			DrawDebugString(GetWorld(), TraceEnd, TEXT("END"), nullptr, FColor::Black, DrawDebugLifeSpawn);
			DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Blue, false, DrawDebugLifeSpawn);
			DrawDebugSphere(GetWorld(), TraceEnd, CrosshairAttackLineSweepShapeRadius, 12, FColor::Red, false, DrawDebugLifeSpawn);
		}

		const FRotator ProjectileRotation = UKismetMathLibrary::FindLookAtRotation(HandLocation, TraceEnd);

		const FTransform SpawnTM = FTransform(ProjectileRotation, HandLocation);

		GetWorld()->SpawnActor<AActor>(ProjectileClass, SpawnTM, SpawnParams);
	}

	StopAction(InstigatorCharacter);
}
