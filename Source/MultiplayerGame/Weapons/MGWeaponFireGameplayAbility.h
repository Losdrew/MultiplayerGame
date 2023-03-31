// Copyright Artem Volovyi. All Rights Reserved.

#pragma once

#include "MGRangedWeaponGameplayAbility.h"
#include "MGWeaponFireGameplayAbility.generated.h"

/**
 * UMGWeaponFireGameplayAbility
 *
 *	An ability for firing a ranged weapon
 */
UCLASS()
class UMGWeaponFireGameplayAbility : public UMGRangedWeaponGameplayAbility
{
	GENERATED_BODY()

public:

	virtual bool CanActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual bool CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;
};
