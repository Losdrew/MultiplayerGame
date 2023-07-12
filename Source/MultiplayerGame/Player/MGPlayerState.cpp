// Copyright Artem Volovyi. All Rights Reserved.


#include "MGPlayerState.h"

#include "MGAbilitySet.h"
#include "Net/UnrealNetwork.h"

AMGPlayerState::AMGPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UMGAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

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

void AMGPlayerState::OnPlayerPawnKilled(AActor* KillerActor)
{
	// Execute only on server
	if (!HasAuthority())
	{
		return;
	}

	if (AMGPlayerState* KillerPlayerState = Cast<AMGPlayerState>(KillerActor))
	{
		KillerPlayerState->PlayerKills++;
		KillerPlayerState->OnRep_KillDeathCount();
	}

	PlayerDeaths++;
	OnRep_KillDeathCount();
}

void AMGPlayerState::OnRep_KillDeathCount()
{
	OnKillDeathCountChanged.Broadcast(PlayerKills, PlayerDeaths);
}
