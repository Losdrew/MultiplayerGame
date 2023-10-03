// Copyright Artem Volovyi. All Rights Reserved.

#pragma once

#include "GameFramework/GameState.h"

#include "MGGameState.generated.h"

class UMGAbilitySet;
class UMGHUDDataAsset;
struct FGameplayEffectContextHandle;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMGGameState_PlayerConnection, AController*, Controller);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMGGameState_MatchStateChanged, FName, NewMatchState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FMGGameState_PlayerKilled, AActor*, KillerActor, AActor*, AssistActor, AActor*, KilledActor, const FGameplayEffectContextHandle&, DamageContext);

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

	void SetMatchDuration(int32 NewDuration) { CurrentMatchDuration = NewDuration; }

	void HandlePostLogin(AController* Controller);
	void HandleLogout(AController* Controller);

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void MulticastOnPlayerKilled(AActor* KillerActor, AActor* AssistActor, AActor* KilledActor, const FGameplayEffectContextHandle& DamageContext);

public:

	UPROPERTY(BlueprintAssignable)
	FMGGameState_PlayerConnection OnPostLogin;

	UPROPERTY(BlueprintAssignable)
	FMGGameState_PlayerConnection OnLogout;

	UPROPERTY(BlueprintAssignable)
    FMGGameState_MatchStateChanged OnMatchStateChanged;

	UPROPERTY(BlueprintAssignable)
	FMGGameState_PlayerKilled OnPlayerKilled;

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

	// Data asset containing ability sets granted by this gamemode
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
    TArray<TObjectPtr<UMGAbilitySet>> AbilitySets;
};
