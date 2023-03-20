// Copyright Artem Volovyi. All Rights Reserved.


#include "MGPlayerState.h"

#include "MGAbilitySet.h"

AMGPlayerState::AMGPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UMGAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AbilitySet = CreateDefaultSubobject<UMGAbilitySet>(TEXT("AbilitySet"));

	// AbilitySystemComponent needs to be updated at a high frequency
	NetUpdateFrequency = 100.0f;
}

void AMGPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	AbilitySystemComponent->InitAbilityActorInfo(this, GetPawn());

	AbilitySet->GiveToAbilitySystem(AbilitySystemComponent, nullptr);
}
