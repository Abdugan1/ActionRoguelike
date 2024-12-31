// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "GameFramework/GameModeBase.h"
#include "SGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API ASGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	ASGameModeBase();

	void StartPlay() override;

	UFUNCTION(Exec)
	void KillAll();

protected:
	UFUNCTION()
	void SpawnBotsTimerElapsed();

	UFUNCTION()
	void OnQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);

protected:
	UPROPERTY(EditDefaultsOnly, Category="AI")
	float SpawnTimerInterval;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UEnvQuery* SpawnBotQuery;

	UPROPERTY(EditDefaultsOnly, Category="AI")
	TSubclassOf<AActor> MinionClass;

	UPROPERTY(EditDefaultsOnly, Category="AI")
	int MaxBotCount;

	UPROPERTY(EditDefaultsOnly, Category="AI")
	UCurveFloat* DifficultyCurve;


	FTimerHandle TimerHandle_SpawnBots;
};
