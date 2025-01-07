// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameModeBase.h"

#include "EngineUtils.h"
#include "SAttributeComponent.h"
#include "SCharacter.h"
#include "SPlayerState.h"
#include "SPowerupActor.h"
#include "AI/SAICharacter.h"
#include "EnvironmentQuery/EnvQueryManager.h"

static TAutoConsoleVariable<bool> CVarSpawnBots{TEXT("su.SpawnBots"), true, TEXT("Enable spawning of bots via timer"), ECVF_Cheat};

static TAutoConsoleVariable<float> CVarCreditsGrantAmount{ TEXT("su.CreditsGrantAmount"), 20, TEXT("Credits grant amount per a kill"), ECVF_Cheat };


ASGameModeBase::ASGameModeBase()
{
	DesiredPowerupCount = 10;
	RequiredPowerupDistance = 2000;

	SpawnTimerInterval = 2.0f;

	MaxBotCount = 10;
}


void ASGameModeBase::StartPlay()
{
	Super::StartPlay();

	// Bot Spawn Timer
	GetWorldTimerManager().SetTimer(
		TimerHandle_SpawnBots, 
		this, 
		&ASGameModeBase::SpawnBotsTimerElapsed, 
		SpawnTimerInterval, 
		true
	);

	// Powerups Spawn
	UEnvQueryInstanceBlueprintWrapper* PowerupsSpawnQueryInstance = UEnvQueryManager::RunEQSQuery(
		this,
		SpawnPowerupsQuery,
		this,
		EEnvQueryRunMode::AllMatching,
		nullptr
	);

	if (ensure(PowerupsSpawnQueryInstance))
	{
		PowerupsSpawnQueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &ASGameModeBase::OnSpawnPowerupsQueryCompleted);
	}
}


void ASGameModeBase::OnActorKilled(AActor* VictimActor, AActor* Killer)
{
	// If the Killer is the Player, grant Credits
	if (ASCharacter *Player = Cast<ASCharacter>(Killer))
	{
		ASPlayerState* PlayerState = ASPlayerState::GetPlayerStateOfPawn(Player);
		if (ensure(PlayerState))
		{
			const float CreditGrantAmount = CVarCreditsGrantAmount.GetValueOnGameThread();
			PlayerState->ApplyCreditsChange(CreditGrantAmount);
			UE_LOG(LogTemp, Log, TEXT("Granting %f credits for killing. Credits: %f"), CreditGrantAmount, PlayerState->GetCredits());
		}
	}


	ASCharacter* Player = Cast<ASCharacter>(VictimActor);
	if (Player)
	{
		FTimerHandle TimerHandle_RespawnDelay;

		FTimerDelegate Delegate;
		Delegate.BindUObject(this, &ASGameModeBase::RespawnPlayerElapsed, Player->GetController());

		float RespawnDelay = 2.0f;
		GetWorldTimerManager().SetTimer(TimerHandle_RespawnDelay, Delegate, RespawnDelay, false);
	}

	UE_LOG(LogTemp, Log, TEXT("OnActorKilled: Vicim: %s, Killer: %s"), *GetNameSafe(VictimActor), *GetNameSafe(Killer));
}


void ASGameModeBase::KillAll()
{
	for (TActorIterator<ASAICharacter> It(GetWorld()); It; ++It)
	{
		ASAICharacter* Bot = *It;
		USAttributeComponent* AttributeComp = Cast<USAttributeComponent>(
			Bot->GetComponentByClass(USAttributeComponent::StaticClass())
		);

		if (ensure(AttributeComp) && AttributeComp->IsAlive())
		{
			AttributeComp->Kill(this); // @todo pass in player? for kill credit
		}
	}
}


void ASGameModeBase::SpawnBotsTimerElapsed()
{
	// Allowed to spawn?
	if (!CVarSpawnBots.GetValueOnGameThread())
	{
		UE_LOG(LogTemp, Warning, TEXT("Bot spawning disable via cvar 'CVarSpawnBots'"));
		return;
	}

	int32 NumOfAliveBots = 0;
	for (TActorIterator<ASAICharacter> It(GetWorld()); It; ++It)
	{
		ASAICharacter* Bot = *It;
		USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(Bot);

		if (ensure(AttributeComp) && AttributeComp->IsAlive())
		{
			NumOfAliveBots++;
		}
	}

	UE_LOG(LogTemp, Log, TEXT("Found %i alive bots."), NumOfAliveBots);

	const int32 CurrentMaxBotCount = DifficultyCurve ? DifficultyCurve->GetFloatValue(GetWorld()->TimeSeconds) : MaxBotCount;

	if (NumOfAliveBots >= CurrentMaxBotCount)
	{
		UE_LOG(LogTemp, Log, TEXT("AI maximum bot capacity reached: %i. Skipping bot spawn"), CurrentMaxBotCount);
		return;
	}

	UEnvQueryInstanceBlueprintWrapper* QueryInstance = UEnvQueryManager::RunEQSQuery(
		this,
		SpawnBotQuery, 
		this, 
		EEnvQueryRunMode::RandomBest5Pct,
		nullptr
	);

	if (ensure(QueryInstance))
	{
		QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &ASGameModeBase::OnSpawnBotQueryCompleted);
	}
}


void ASGameModeBase::OnSpawnBotQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance,
	EEnvQueryStatus::Type QueryStatus)
{
	if (QueryStatus != EEnvQueryStatus::Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawn bot EQS Query Failed!"));
		return;
	}

	TArray<FVector> Locations = QueryInstance->GetResultsAsLocations();

	if (Locations.Num() > 0)
	{
		GetWorld()->SpawnActor<AActor>(MinionClass, Locations[0], FRotator::ZeroRotator);

		DrawDebugSphere(GetWorld(), Locations[0], 50.f, 20, FColor::Blue, false, 60.0f);
	}
}


void ASGameModeBase::OnSpawnPowerupsQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance,
	EEnvQueryStatus::Type QueryStatus)
{
	if (QueryStatus != EEnvQueryStatus::Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawn coin EQS Query Failed!"));
		return;
	}

	TArray<FVector> Locations = QueryInstance->GetResultsAsLocations();
	TArray<FVector> UsedLocations;

	int32 SpawnCounter = 0;
	while (SpawnCounter < DesiredPowerupCount && Locations.Num() > 0)
	{
		const int32 RandomLocationIndex = FMath::RandRange(0, Locations.Num() - 1);

		const FVector PickedLocation = Locations[RandomLocationIndex];

		Locations.RemoveAt(RandomLocationIndex);

		bool bValidLocation = true;
		for (FVector OtherLocation : UsedLocations)
		{
			const float DistanceTo = (PickedLocation - OtherLocation).Size();

			if (DistanceTo < RequiredPowerupDistance)
			{
				// Show skipped locations due to distance
				DrawDebugSphere(GetWorld(), PickedLocation, 50.0f, 20, FColor::Purple, false, 10.0f);

				bValidLocation = false;
				break;
			}
		}

		if (!bValidLocation)
		{
			continue;
		}

		const int32 RandomClassIndex = FMath::RandRange(0, PowerupClasses.Num() - 1);

		TSubclassOf<ASPowerupActor> RandomPowerupClass = PowerupClasses[RandomClassIndex];

		GetWorld()->SpawnActor<AActor>(RandomPowerupClass, PickedLocation, FRotator::ZeroRotator);

		UsedLocations.Add(PickedLocation);
		SpawnCounter++;
	}
}


void ASGameModeBase::RespawnPlayerElapsed(AController* Controller)
{
	if (ensure(Controller))
	{
		Controller->UnPossess();
		RestartPlayer(Controller);
	}
}
