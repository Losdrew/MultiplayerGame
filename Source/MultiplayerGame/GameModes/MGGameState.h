// Copyright Artem Volovyi. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "MGHUDDataAsset.h"
#include "MGGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMGGameState_MatchStateChanged, FName, NewMatchState);

/**
 * AMGGameState
 *
 *	The base game state class used by this project
 */
UCLASS()
class AMGGameState : public AGameState
{
	GENERATED_BODY()

public:

	//~AGameState interface
	virtual void OnRep_MatchState() override;
	virtual void DefaultTimer() override;
	//~End of AGameMode interface

	virtual void ResetElapsedTime();

public:

	UPROPERTY(BlueprintAssignable)
    FMGGameState_MatchStateChanged OnMatchStateChanged;

public:
	// Match duration in the current match state
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Gameplay")
    int CurrentMatchDuration;

	// Remaining time of the current match state
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Gameplay")
    int CurrentRemainingTime;

	// Data asset containing widgets for this gamemode's HUD
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
    TObjectPtr<UMGHUDDataAsset> HUDDataAsset;

};
