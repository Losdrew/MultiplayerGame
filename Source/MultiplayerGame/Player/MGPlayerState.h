// Copyright Artem Volovyi. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "MGAbilitySystemComponent.h"
#include "MGPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMGPlayerState_StatChanged, int, PlayerKills);

class UMGAbilitySet;

/**
 * AMGPlayerState
 *
 *	The base player state class used by this project
 */
UCLASS()
class AMGPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:

	AMGPlayerState();

	//~AActor interface
	virtual void PostInitializeComponents() override;
	//~End of AActor interface

	//~IAbilitySystemInterface interface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	//~End of IAbilitySystemInterface interface

	UMGAbilitySystemComponent* GetMGAbilitySystemComponent() const;

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Abilities")
	UMGAbilitySet* AbilitySet;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities", Meta = (AllowPrivateAccess = "true"))
	UMGAbilitySystemComponent* AbilitySystemComponent;

public:

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void AddPlayerKills(); 

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void AddPlayerDeaths();

	UFUNCTION(BlueprintCallable)
	int32 GetPlayerKills() { return PlayerKills; }

	UFUNCTION(BlueprintCallable)
	int32 GetPlayerDeaths() { return PlayerDeaths; }

private:

	UPROPERTY(ReplicatedUsing=OnRep_PlayerKills)
	int32 PlayerKills;

	UPROPERTY(ReplicatedUsing=OnRep_PlayerDeaths)
	int32 PlayerDeaths;

protected:

	UFUNCTION()
	void OnRep_PlayerKills();

	UFUNCTION()
	void OnRep_PlayerDeaths();

public:

	UPROPERTY()
	FMGPlayerState_StatChanged OnPlayerKillsChanged;

	UPROPERTY()
	FMGPlayerState_StatChanged OnPlayerDeathsChanged;
};
