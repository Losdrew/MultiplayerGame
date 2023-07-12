// Copyright Artem Volovyi. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
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

	virtual void OnRep_MatchState() override;

public:

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Gameplay")
    int MatchDuration;

	UPROPERTY(BlueprintAssignable)
    FMGGameState_MatchStateChanged OnMatchStateChanged;
};
