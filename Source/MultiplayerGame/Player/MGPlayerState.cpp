// Copyright Artem Volovyi. All Rights Reserved.


#include "MGPlayerState.h"

#include "MGAbilitySet.h"
#include "Net/UnrealNetwork.h"

AMGPlayerState::AMGPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UMGAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Full);

	AbilitySet = CreateDefaultSubobject<UMGAbilitySet>(TEXT("AbilitySet"));

	// AbilitySystemComponent needs to be updated at a high frequency
	NetUpdateFrequency = 100.0f;
}

void AMGPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION_NOTIFY(AMGPlayerState, PlayerKills, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(AMGPlayerState, PlayerDeaths, COND_None, REPNOTIFY_Always);
}

void AMGPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	AbilitySystemComponent->InitAbilityActorInfo(this, GetPawn());

	AbilitySet->GiveToAbilitySystem(AbilitySystemComponent, nullptr);
}

UAbilitySystemComponent* AMGPlayerState::GetAbilitySystemComponent() const
{
	 return GetMGAbilitySystemComponent();
}

UMGAbilitySystemComponent* AMGPlayerState::GetMGAbilitySystemComponent() const
{
	 return AbilitySystemComponent;
}

void AMGPlayerState::ResetPlayerStats()
{
	PlayerKills = 0;
	OnRep_PlayerKills();

	PlayerDeaths = 0;
	OnRep_PlayerDeaths();
}

void AMGPlayerState::AddPlayerKills()
{
	PlayerKills++;
	OnRep_PlayerKills();
}

void AMGPlayerState::AddPlayerDeaths()
{
	PlayerDeaths++;
	OnRep_PlayerDeaths();
}


void AMGPlayerState::OnRep_PlayerKills()
{
	OnPlayerKillsChanged.Broadcast(PlayerKills);
}

void AMGPlayerState::OnRep_PlayerDeaths()
{
	OnPlayerDeathsChanged.Broadcast(PlayerDeaths);
}
