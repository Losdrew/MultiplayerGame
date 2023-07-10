// Copyright Artem Volovyi. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "MGGameMode.generated.h"

/**
 * AMGGameMode
 *
 *	The base game mode class used by this project
 */
UCLASS()
class AMGGameMode : public AGameMode
{
	GENERATED_BODY()

public:

	virtual void BeginPlay() override;

	virtual bool ReadyToStartMatch_Implementation() override;

	UFUNCTION(BlueprintCallable)
	void RequestPlayerRestartNextFrame(AController* Controller, bool bForceReset);

protected:

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay")
    int MatchDuration;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay")
    int MinPlayers;
};
