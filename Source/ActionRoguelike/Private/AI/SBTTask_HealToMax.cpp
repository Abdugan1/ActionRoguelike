// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SBTTask_HealToMax.h"

#include "AIController.h"
#include "SAttributeComponent.h"

EBTNodeResult::Type USBTTask_HealToMax::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* MyController = OwnerComp.GetAIOwner();
	if (ensure(MyController))
	{
		APawn* MyPawn = MyController->GetPawn();
		if (!MyPawn)
		{
			return EBTNodeResult::Failed;
		}

		USAttributeComponent* AttributeComp = Cast<USAttributeComponent>(
			MyPawn->GetComponentByClass(USAttributeComponent::StaticClass())
		);

		if (ensure(AttributeComp))
		{
			AttributeComp->ApplyHealthChange(MyPawn, AttributeComp->GetMaxHealth());
			return EBTNodeResult::Succeeded;
		}
		else
		{
			return EBTNodeResult::Failed;
		}
		
	}

	return EBTNodeResult::Failed;
}
