// Copyright Artem Volovyi. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MGStatsComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMGPlayerState_StatChanged, int32, Stat);

/**
 * UMGStatsComponent
 *
 *	Component used to manage player stats (kills, deaths, etc.)
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UMGStatsComponent : public UActorComponent
{
    GENERATED_BODY()

public:    

    UMGStatsComponent();

    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
    virtual void ResetPlayerStats(); 

    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
    void AddPlayerKills();

    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
    void AddPlayerAssists(); 

    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
    void AddPlayerDeaths();

    UFUNCTION(BlueprintPure)
    int32 GetPlayerKills() const { return PlayerKills; }

    UFUNCTION(BlueprintPure)
    int32 GetPlayerAssists() const { return PlayerAssists; }

    UFUNCTION(BlueprintPure)
    int32 GetPlayerDeaths() const { return PlayerDeaths; }

public:

    UPROPERTY(BlueprintAssignable)
    FMGPlayerState_StatChanged OnPlayerKillsChanged;

    UPROPERTY(BlueprintAssignable)
    FMGPlayerState_StatChanged OnPlayerAssistsChanged;

    UPROPERTY(BlueprintAssignable)
    FMGPlayerState_StatChanged OnPlayerDeathsChanged;

protected:

	UFUNCTION()
    void OnRep_PlayerKills();
    
    UFUNCTION()
    void OnRep_PlayerAssists();
    
    UFUNCTION()
    void OnRep_PlayerDeaths();

private:

    UPROPERTY(ReplicatedUsing=OnRep_PlayerKills)
    int32 PlayerKills;
    
    UPROPERTY(ReplicatedUsing=OnRep_PlayerAssists)
    int32 PlayerAssists;
    
    UPROPERTY(ReplicatedUsing=OnRep_PlayerDeaths)
    int32 PlayerDeaths;
};
