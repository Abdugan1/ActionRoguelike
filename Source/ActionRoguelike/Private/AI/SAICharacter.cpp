// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SAICharacter.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/PawnSensingComponent.h"


ASAICharacter::ASAICharacter()
{
	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>("PawnSensingComp");
}


void ASAICharacter::OnPawnSeen(APawn* Pawn)
{
	AAIController *MyController = Cast<AAIController>(GetController());
	if (MyController)
	{
		UBlackboardComponent* BlackboardComp = MyController->GetBlackboardComponent();

		BlackboardComp->SetValueAsObject("TargetActor", Pawn);

		DrawDebugString(
			GetWorld(), GetActorLocation(),
			"PLAYER SPOTTED", nullptr, 
			FColor::White, 4.0f, true
		);
	}
}


void ASAICharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	PawnSensingComp->OnSeePawn.AddDynamic(this, &ASAICharacter::OnPawnSeen);
}
