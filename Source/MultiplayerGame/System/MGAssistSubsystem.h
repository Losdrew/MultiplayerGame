// Copyright Artem Volovyi. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayEffectTypes.h"
#include "MGAssistSubsystem.generated.h"

/**
 * FMGPlayerDamageDealt
 *
 *  Tracks the damage done to a player by other players
 */
USTRUCT()
struct FMGPlayerDamageDealt
{
	GENERATED_BODY()

	// Map of damager to damage dealt
	UPROPERTY(Transient)
	TMap<TObjectPtr<APlayerState>, float> AccumulatedDamageByPlayer;
};

/**
 * UMGAssistSubsystem
 *
 *  Tracks assists (dealing damage to another player without finishing them)
 */
UCLASS()
class UMGAssistSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	// Records the damage history on target
	UFUNCTION()
	void OnPlayerReceiveDamage(AActor* Instigator, AActor* Target, const FGameplayEffectSpec& EffectSpec, float Magnitude);

	// Finds all players who dealt damage to the KilledActor but are not KillerActor
	UFUNCTION()
	TArray<APlayerState*> FindKillAssistPlayers(AActor* KillerActor, AActor* KilledActor);

	// Finds the player who dealt the most assist damage to the KilledActor
	UFUNCTION()
	APlayerState* FindMaxDamageAssistPlayer(AActor* KillerActor, AActor* KilledActor);

	UFUNCTION()
	void ClearDamageHistoryForPlayer(AActor* Player);

private:
	// Map of player to damage dealt to them
	UPROPERTY(Transient)
	TMap<TObjectPtr<APlayerState>, FMGPlayerDamageDealt> DamageHistory;
};
