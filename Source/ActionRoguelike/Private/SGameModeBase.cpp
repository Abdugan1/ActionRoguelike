// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameModeBase.h"

#include "EngineUtils.h"
#include "SActionComponent.h"
#include "SAttributeComponent.h"
#include "SCharacter.h"
#include "SMonsterData.h"
#include "SPlayerState.h"
#include "SPowerupActor.h"
#include "SSaveGame.h"
#include "ActionRoguelike/ActionRoguelike.h"
#include "AI/SAICharacter.h"
#include "Engine/AssetManager.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"

static TAutoConsoleVariable<bool> CVarSpawnBots{TEXT("su.SpawnBots"), true, TEXT("Enable spawning of bots via timer"), ECVF_Cheat};

static TAutoConsoleVariable<int32> CVarCreditsGrantAmount{ TEXT("su.CreditsGrantAmount"), 20, TEXT("Credits grant amount per a kill"), ECVF_Cheat };


ASGameModeBase::ASGameModeBase()
{
	DesiredPowerupCount = 10;
	RequiredPowerupDistance = 2000;

	SpawnTimerInterval = 2.0f;

	MaxBotCount = 10;

	SlotName = "SaveGame01";
}


void ASGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	FString SelectedSaveSlot = UGameplayStatics::ParseOption(Options, "SaveGame");
	if (SelectedSaveSlot.Len() > 0)
	{
		SlotName = SelectedSaveSlot;
	}

	// Tom says it's gonna crash in UE5. Not sure though...
	LoadSaveGame();
}


void ASGameModeBase::StartPlay()
{
	Super::StartPlay();

	//LoadSaveGame();

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


// Warning: It seems this runs before BeginPlay. So if Tom's saying of potential error is true
// and if I moved LoadSaveGame in BeginPlay, then CurrentSaveGame is not valid, which will not
// load the data!
void ASGameModeBase::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	ASPlayerState* PS = NewPlayer->GetPlayerState<ASPlayerState>();
	if (ensure(PS))
	{
		PS->LoadPlayerState(CurrentSaveGame);
	}


	// Calling this before loading the game calls bunch of functions, including the PlayerController::BeginPlayingState
	// which we are using to init the Main HUD. So a whole of things will not work properly.
	// So loading first, doing this second
	Super::HandleStartingNewPlayer_Implementation(NewPlayer);
}


void ASGameModeBase::OnActorKilled(AActor* VictimActor, AActor* Killer)
{
	// If the Killer is the Player, grant Credits
	APawn* KillerPawn = Cast<ASCharacter>(Killer);
	if (KillerPawn && KillerPawn != VictimActor)
	{
		ASPlayerState* PlayerState = ASPlayerState::GetPlayerStateOfPawn(KillerPawn);
		if (PlayerState)
		{
			const int32 CreditGrantAmount = CVarCreditsGrantAmount.GetValueOnGameThread();
			PlayerState->ApplyCreditsChange(CreditGrantAmount);
		}
	}


	// Respawn after delay
	ASCharacter* Player = Cast<ASCharacter>(VictimActor);
	if (Player)
	{
		FTimerHandle TimerHandle_RespawnDelay;

		FTimerDelegate Delegate;
		Delegate.BindUObject(this, &ASGameModeBase::RespawnPlayerElapsed, Player->GetController());

		float RespawnDelay = 2.0f;
		GetWorldTimerManager().SetTimer(TimerHandle_RespawnDelay, Delegate, RespawnDelay, false);
	}
}


void ASGameModeBase::WriteSaveGame()
{
	for (int32 i = 0; i < GameState->PlayerArray.Num(); i++)
	{
		ASPlayerState* PS = Cast<ASPlayerState>(GameState->PlayerArray[i]);
		if (PS)
		{
			PS->SavePlayerState(CurrentSaveGame);
			UE_LOG(LogTemp, Warning, TEXT("Keep in mind this saving is single player only!"));
			break; // Single Player only at this point
		}
	}

	CurrentSaveGame->SavedActors.Empty();

	// Iterate the entire world of actors
	for (FActorIterator It(GetWorld()); It; ++It)
	{
		AActor* Actor = *It;
		if (!Actor->Implements<USGameplayInterface>())
		{
			continue;
		}

		FActorSaveData ActorData;
		ActorData.ActorName = Actor->GetName();
		ActorData.Transform = Actor->GetActorTransform();

		// Pass the array to fill with data from Actor
		FMemoryWriter MemWriter(ActorData.ByteData);

		FObjectAndNameAsStringProxyArchive Ar(MemWriter, true);
		// Find only variables with UPROPERTY(SaveGame)
		Ar.ArIsSaveGame = true;
		// Converts Actor's SaveGame UPROPERTies into binary array
		Actor->Serialize(Ar);

		CurrentSaveGame->SavedActors.Add(ActorData);
	}

	UGameplayStatics::SaveGameToSlot(CurrentSaveGame, SlotName, 0);
}


void ASGameModeBase::LoadSaveGame()
{
	if (UGameplayStatics::DoesSaveGameExist(SlotName, 0))
	{
		CurrentSaveGame = Cast<USSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, 0));
		if (CurrentSaveGame == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to load SaveGame Data."));
			return;
		}
		UE_LOG(LogTemp, Log, TEXT("Loaded SaveGame Data."));

		// Iterate the entire world of actors
		for (FActorIterator It(GetWorld()); It; ++It)
		{
			AActor* Actor = *It;
			if (!Actor->Implements<USGameplayInterface>())
			{
				continue;
			}

			for (FActorSaveData ActorData : CurrentSaveGame->SavedActors)
			{
				if (ActorData.ActorName == Actor->GetName())
				{
					Actor->SetActorTransform(ActorData.Transform);

					// Pass the array to fill with data from Actor
					FMemoryReader MemReader(ActorData.ByteData);

					FObjectAndNameAsStringProxyArchive Ar(MemReader, true);
					// Find only variables with UPROPERTY(SaveGame)
					Ar.ArIsSaveGame = true;
					// Convert binary array back into actor's variables
					Actor->Serialize(Ar);

					ISGameplayInterface::Execute_OnActorLoaded(Actor);

					break;
				}
			}

			FActorSaveData ActorData;
			ActorData.ActorName = Actor->GetName();
			ActorData.Transform = Actor->GetActorTransform();

			CurrentSaveGame->SavedActors.Add(ActorData);
		}
	}
	else
	{
		CurrentSaveGame = Cast<USSaveGame>(UGameplayStatics::CreateSaveGameObject(USSaveGame::StaticClass()));
		UE_LOG(LogTemp, Log, TEXT("Created SaveGame Data."));
	}
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
		//UE_LOG(LogTemp, Warning, TEXT("Bot spawning disable via cvar 'CVarSpawnBots'"));
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
		if (ensure(MonsterTable))
		{
			TArray<FMonsterInfoRow*> Rows;
			MonsterTable->GetAllRows("", Rows);

			const int32 RandomIndex = FMath::RandRange(0, Rows.Num() - 1);
			FMonsterInfoRow* SelectedRow = Rows[RandomIndex];

			UAssetManager* Manager = UAssetManager::GetIfValid();
			if (Manager)
			{
				LogOnScreen(this, "Loading monster...", FColor::Green);
				TArray<FName> Bundles;

				FStreamableDelegate Delegate = FStreamableDelegate::CreateUObject(this, &ASGameModeBase::OnMonsterLoaded, SelectedRow->MonsterId, Locations[0]);

				Manager->LoadPrimaryAsset(SelectedRow->MonsterId, Bundles, Delegate);
			}
		}
	}
}


void ASGameModeBase::OnMonsterLoaded(FPrimaryAssetId PrimaryAssetId, FVector SpawnLocation)
{
	LogOnScreen(this, "Finished loading.", FColor::Green);
	UAssetManager* Manager = UAssetManager::GetIfValid();
	if (Manager)
	{
		USMonsterData* MonsterData = Cast<USMonsterData>(Manager->GetPrimaryAssetObject(PrimaryAssetId));
		if (MonsterData)
		{
			AActor* NewBot = GetWorld()->SpawnActor<AActor>(MonsterData->MonsterClass, SpawnLocation, FRotator::ZeroRotator);

			if (NewBot)
			{
				LogOnScreen(this, FString::Printf(TEXT("Spawned enemy: %s (%s)"), *GetNameSafe(NewBot), *GetNameSafe(MonsterData)));

				USActionComponent* ActionComp = USActionComponent::GetActionComponent(NewBot);
				if (ActionComp)
				{
					for (TSubclassOf<USAction> ActionClass : MonsterData->Actions)
					{
						ActionComp->AddAction(NewBot, ActionClass);
					}
				}
			}
		}
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
