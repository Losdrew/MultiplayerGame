// Copyright Artem Volovyi. All Rights Reserved.


#include "MGPlayerState.h"

#include "MGAbilitySet.h"
#include "MGGameState.h"
#include "Net/UnrealNetwork.h"
#include "MGAbilitySystemComponent.h"
#include "MGGameplayTagStack.h"

AMGPlayerState::AMGPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UMGAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Full);

	// AbilitySystemComponent needs to be updated at a high frequency
	NetUpdateFrequency = 100.0f;

	StatTags.OnGameplayTagStackChanged.AddDynamic(this, &ThisClass::OnTagStackChanged);
}

void AMGPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMGPlayerState, StatTags);
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

void AMGPlayerState::OnTagStackChanged(FGameplayTag Tag, int32 StackCount)
{
	OnStatTagChanged.Broadcast(Tag, StackCount);
}

void AMGPlayerState::AddStatTagStack(FGameplayTag Tag, int32 StackCount)
{
	StatTags.AddStack(Tag, StackCount);
}

void AMGPlayerState::RemoveStatTagStack(FGameplayTag Tag, int32 StackCount)
{
	StatTags.RemoveStack(Tag, StackCount);
}

int32 AMGPlayerState::GetStatTagStackCount(FGameplayTag Tag) const
{
	return StatTags.GetStackCount(Tag);
}

bool AMGPlayerState::HasStatTag(FGameplayTag Tag) const
{
	return StatTags.ContainsTag(Tag);
}

void AMGPlayerState::ResetAllStats()
{
	StatTags.RemoveAllStacks();
}
