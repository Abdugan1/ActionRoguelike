// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameModeBase.h"

#include "EngineUtils.h"
#include "SAttributeComponent.h"
#include "AI/SAICharacter.h"
#include "EnvironmentQuery/EnvQueryManager.h"


ASGameModeBase::ASGameModeBase()
{
	SpawnTimerInterval = 2.0f;

	MaxBotCount = 10;
}


void ASGameModeBase::StartPlay()
{
	Super::StartPlay();

	GetWorldTimerManager().SetTimer(
		TimerHandle_SpawnBots, 
		this, 
		&ASGameModeBase::SpawnBotsTimerElapsed, 
		SpawnTimerInterval, 
		true
	);
}


void ASGameModeBase::SpawnBotsTimerElapsed()
{
	UEnvQueryInstanceBlueprintWrapper* QueryInstance = UEnvQueryManager::RunEQSQuery(
		this,
		SpawnBotQuery, 
		this, 
		EEnvQueryRunMode::RandomBest5Pct,
		nullptr
	);

	if (ensure(QueryInstance))
	{
		QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &ASGameModeBase::OnQueryCompleted);
	}
}


void ASGameModeBase::OnQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance,
	EEnvQueryStatus::Type QueryStatus)
{
	if (QueryStatus != EEnvQueryStatus::Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawn bot EQS Query Failed!"));
		return;
	}

	int32 NumOfAliveBots = 0;
	for (TActorIterator<ASAICharacter> It(GetWorld()); It; ++It)
	{
		ASAICharacter* Bot = *It;
		USAttributeComponent *AttributeComp =  Cast<USAttributeComponent>(
			Bot->GetComponentByClass(USAttributeComponent::StaticClass())
		);

		if (ensure(AttributeComp) && AttributeComp->IsAlive())
		{
			NumOfAliveBots++;
		}
	}

	const int CurrentMaxBotCount = DifficultyCurve ? DifficultyCurve->GetFloatValue(GetWorld()->TimeSeconds) : MaxBotCount;

	if (NumOfAliveBots >= CurrentMaxBotCount)
	{
		return;
	}

	TArray<FVector> Locations = QueryInstance->GetResultsAsLocations();

	if (Locations.Num() > 0)
	{
		GetWorld()->SpawnActor<AActor>(MinionClass, Locations[0], FRotator::ZeroRotator);
	}
}
