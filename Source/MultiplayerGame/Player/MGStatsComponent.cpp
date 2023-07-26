// Copyright Artem Volovyi. All Rights Reserved.


#include "Player/MGStatsComponent.h"

#include "Net/UnrealNetwork.h"

UMGStatsComponent::UMGStatsComponent()
{
	UMGStatsComponent::ResetPlayerStats();
}

void UMGStatsComponent::ResetPlayerStats()
{
	PlayerKills = 0;
	OnRep_PlayerKills();

	PlayerAssists = 0;
	OnRep_PlayerAssists();

	PlayerDeaths = 0;
	OnRep_PlayerDeaths();
}

void UMGStatsComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UMGStatsComponent, PlayerKills, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMGStatsComponent, PlayerAssists, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMGStatsComponent, PlayerDeaths, COND_None, REPNOTIFY_Always);
}

void UMGStatsComponent::AddPlayerKills()
{
	PlayerKills++;
	OnRep_PlayerKills();
}

void UMGStatsComponent::AddPlayerAssists()
{
	PlayerAssists++;
	OnRep_PlayerAssists();
}

void UMGStatsComponent::AddPlayerDeaths()
{
	PlayerDeaths++;
	OnRep_PlayerDeaths();
}

void UMGStatsComponent::OnRep_PlayerKills()
{
	OnPlayerKillsChanged.Broadcast(PlayerKills);
}

void UMGStatsComponent::OnRep_PlayerAssists()
{
	OnPlayerAssistsChanged.Broadcast(PlayerAssists);
}

void UMGStatsComponent::OnRep_PlayerDeaths()
{
	OnPlayerDeathsChanged.Broadcast(PlayerDeaths);
}
