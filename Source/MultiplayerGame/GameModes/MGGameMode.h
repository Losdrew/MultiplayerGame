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

	virtual void InitGameState() override;

	virtual void RestartPlayer(AController* NewPlayer) override;

	UFUNCTION(BlueprintCallable)
	void RequestPlayerRestartNextFrame(AController* Controller, bool bForceReset);

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="OnPlayerKilled"))
	void K2_OnPlayerKilled(AActor* KillerPlayer, AActor* KilledPlayer);

protected:

	virtual bool ReadyToStartMatch_Implementation() override;

	UFUNCTION()
	virtual void OnPlayerKilled(AActor* KillerPlayer, AActor* KilledPlayer);

protected:

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay")
    int32 MatchDuration;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay")
    int32 MinPlayers;
};
