// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SAICharacter.h"

#include "AIController.h"
#include "BrainComponent.h"
#include "SActionComponent.h"
#include "SAttributeComponent.h"
#include "SCharacter.h"
#include "SWorldUserWidget.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/PawnSensingComponent.h"


ASAICharacter::ASAICharacter()
{
	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>("PawnSensingComp");

	AttributeComp = CreateDefaultSubobject<USAttributeComponent>("AttributeComp");

	ActionComp = CreateDefaultSubobject<USActionComponent>("ActionComp");

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);

	TargetActorKey = "TargetActor";

	HitFlashTimeParamName = "HitFlashTime";
}


void ASAICharacter::OnPawnSeen(APawn* Pawn)
{
	if (GetTargetActor() != Pawn)
	{
		SetTargetActor(Pawn);

		if (ensure(SpottedWidgetClass))
		{
			MulticastShowSpottedUIWidget();
		}
	}
}


void ASAICharacter::OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComponent, float NewHealth,
	float Delta)
{
	//PrintString
	if (Delta < 0.0f)
	{
		if (InstigatorActor != this)
		{
			SetTargetActor(InstigatorActor);
		}

		if (ActiveHealthBar == nullptr)
		{
			ActiveHealthBar = CreateWidget<USWorldUserWidget>(GetWorld(), HealthBarWidgetClass);
			if (ActiveHealthBar)
			{
				ActiveHealthBar->AttachedActor = this;
				ActiveHealthBar->AddToViewport();
			}
		}

		GetMesh()->SetScalarParameterValueOnMaterials(HitFlashTimeParamName, GetWorld()->TimeSeconds);

		if (NewHealth <= 0.0f)
		{
			// Stop BT
			AAIController* MyController = Cast<AAIController>(GetController());
			if (MyController)
			{
				MyController->GetBrainComponent()->StopLogic("Killed");
			}

			// Ragdoll
			GetMesh()->SetAllBodiesSimulatePhysics(true);
			GetMesh()->SetCollisionProfileName("Ragdoll");

			// Disable Collision
			GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			GetCharacterMovement()->DisableMovement();

			// Set lifespan
			SetLifeSpan(10.0f);
		}
	}
}


void ASAICharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	PawnSensingComp->OnSeePawn.AddDynamic(this, &ASAICharacter::OnPawnSeen);

	AttributeComp->OnHealthChanged.AddDynamic(this, &ASAICharacter::OnHealthChanged);
}


void ASAICharacter::SetTargetActor(AActor* NewTarget)
{
	AAIController* MyController = Cast<AAIController>(GetController());
	if (MyController)
	{
		MyController->GetBlackboardComponent()->SetValueAsObject(TargetActorKey, NewTarget);;
	}
}


AActor* ASAICharacter::GetTargetActor() const
{
	AAIController* MyController = Cast<AAIController>(GetController());
	if (MyController)
	{
		return Cast<AActor>(MyController->GetBlackboardComponent()->GetValueAsObject(TargetActorKey));
	}
	return nullptr;
}


void ASAICharacter::MulticastShowSpottedUIWidget_Implementation()
{
	USWorldUserWidget* ActiveSpottedWidget = CreateWidget<USWorldUserWidget>(GetWorld(), SpottedWidgetClass);
	ActiveSpottedWidget->AttachedActor = this;
	ActiveSpottedWidget->AddToViewport(10);
}
