// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SBTService_CheckHealth.h"

#include "AIController.h"
#include "SAttributeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

USBTService_CheckHealth::USBTService_CheckHealth()
{
	LowHealthThreshold = 0.3f;
}

void USBTService_CheckHealth::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	auto BlackboardComp = OwnerComp.GetBlackboardComponent();

	if (ensure(BlackboardComp))
	{
		AAIController* MyController = OwnerComp.GetAIOwner();

		if (ensure(MyController))
		{
			APawn* MyPawn = MyController->GetPawn();

			if (ensure(MyPawn))
			{
				USAttributeComponent *AttributeComp = Cast<USAttributeComponent>(
					MyPawn->GetComponentByClass(USAttributeComponent::StaticClass())
				);
				if (ensure(AttributeComp))
				{
					float HealthScaled = AttributeComp->GetHealthScaled();
					UE_LOG(LogTemp, Warning, TEXT("Health Scaled: %f %f"), HealthScaled, LowHealthThreshold);
					BlackboardComp->SetValueAsBool(
						LowHealthKey.SelectedKeyName, 
						HealthScaled <= LowHealthThreshold
					);
				}
			}
		}
	}
}
