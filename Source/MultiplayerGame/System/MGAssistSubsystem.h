// Copyright Artem Volovyi. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
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

	UFUNCTION()
	void RecordPlayerDamage(AActor* Instigator, AActor* Target, float Magnitude);

	UFUNCTION()
	void GrantAssistsForPlayerKill(AActor* KillerActor, AActor* KilledActor);

private:
	// Map of player to damage dealt to them
	UPROPERTY(Transient)
	TMap<TObjectPtr<APlayerState>, FMGPlayerDamageDealt> DamageHistory;
};
