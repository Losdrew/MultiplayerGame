// Copyright Artem Volovyi. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "MGGameState.generated.h"

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

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Gameplay")
    int MatchDuration;
};
