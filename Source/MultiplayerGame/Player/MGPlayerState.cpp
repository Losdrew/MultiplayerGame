// Copyright Artem Volovyi. All Rights Reserved.


#include "MGPlayerState.h"

#include "MGAbilitySet.h"
#include "MGGameState.h"
#include "MGStatsComponent.h"

AMGPlayerState::AMGPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UMGAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Full);

	// AbilitySystemComponent needs to be updated at a high frequency
	NetUpdateFrequency = 100.0f;

	StatsComponent = CreateDefaultSubobject<UMGStatsComponent>(TEXT("StatsComponent"));
	StatsComponent->SetIsReplicated(true);
}

void AMGPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	AbilitySystemComponent->InitAbilityActorInfo(this, GetPawn());

	if (AMGGameState* GameState = Cast<AMGGameState>(GetWorld()->GetGameState()))
	{
		for (const UMGAbilitySet* AbilitySet : GameState->AbilitySets)
		{
			AbilitySet->GiveToAbilitySystem(AbilitySystemComponent, nullptr);
		}
	}
}

UAbilitySystemComponent* AMGPlayerState::GetAbilitySystemComponent() const
{
	 return GetMGAbilitySystemComponent();
}

UMGAbilitySystemComponent* AMGPlayerState::GetMGAbilitySystemComponent() const
{
	 return AbilitySystemComponent;
}

UMGStatsComponent* AMGPlayerState::GetStatsComponent() const
{
	return StatsComponent;
}
