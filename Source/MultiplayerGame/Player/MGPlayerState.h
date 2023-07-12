// Copyright Artem Volovyi. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "MGAbilitySystemComponent.h"
#include "MGPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMGPlayerState_KillDeathCountChanged, int, PlayerKills, int, PlayerDeaths);

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

	UFUNCTION()
	virtual void OnPlayerPawnKilled(AActor* KillerActor);
	
    UFUNCTION(BlueprintCallable)
    int32 GetPlayerKills() const { return PlayerKills; }

    UFUNCTION(BlueprintCallable)
    int32 GetPlayerDeaths() const { return PlayerDeaths; }

public:

    UPROPERTY(BlueprintAssignable)
    FMGPlayerState_KillDeathCountChanged OnKillDeathCountChanged;

protected:

	UFUNCTION()
    virtual void OnRep_KillDeathCount();

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Abilities")
	UMGAbilitySet* AbilitySet;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities", Meta = (AllowPrivateAccess = "true"))
	UMGAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY(ReplicatedUsing=OnRep_KillDeathCount, BlueprintReadOnly)
	int PlayerKills;

	UPROPERTY(ReplicatedUsing=OnRep_KillDeathCount, BlueprintReadOnly)
	int PlayerDeaths;
};
