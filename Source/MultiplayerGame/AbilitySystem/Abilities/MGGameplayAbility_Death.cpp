// Copyright Artem Volovyi. All Rights Reserved.


#include "MGGameplayAbility_Death.h"

#include "MGAbilitySystemComponent.h"
#include "MGGameplayTags.h"
#include "MGHealthComponent.h"

void UMGGameplayAbility_Death::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	check(ActorInfo);

	UMGAbilitySystemComponent* ASC = CastChecked<UMGAbilitySystemComponent>(ActorInfo->AbilitySystemComponent.Get());

	FGameplayTagContainer AbilityTypesToIgnore;
	AbilityTypesToIgnore.AddTag(MGGameplayTags::Ability_Behavior_SurvivesDeath);

	// Cancel all abilities and block others from starting
	ASC->CancelAbilities(nullptr, &AbilityTypesToIgnore, this);

	SetCanBeCanceled(false);

	StartDeath();

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UMGGameplayAbility_Death::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	check(ActorInfo);

	// Always try to finish the death when the ability ends in case the ability doesn't
	// This won't do anything if the death hasn't been started
	FinishDeath();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UMGGameplayAbility_Death::StartDeath()
{
	if (UMGHealthComponent* HealthComponent = GetAvatarActorFromActorInfo()->FindComponentByClass<UMGHealthComponent>())
	{
		if (HealthComponent->GetDeathState() == EMGDeathState::NotDead)
		{
			HealthComponent->StartDeath();
		}
	}
}

void UMGGameplayAbility_Death::FinishDeath()
{
	if (UMGHealthComponent* HealthComponent = GetAvatarActorFromActorInfo()->FindComponentByClass<UMGHealthComponent>())
	{
		if (HealthComponent->GetDeathState() == EMGDeathState::DeathStarted)
		{
			HealthComponent->FinishDeath();
		}
	}
}
