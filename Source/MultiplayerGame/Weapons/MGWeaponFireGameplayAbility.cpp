// Copyright Artem Volovyi. All Rights Reserved.


#include "MGWeaponFireGameplayAbility.h"

bool UMGWeaponFireGameplayAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	bool bResult = Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);

	if (bResult)
	{
		if (GetWeaponInstance() == nullptr)
		{
			bResult = false;
		}
	}

	return bResult;
}

bool UMGWeaponFireGameplayAbility::CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const
{
	bool bCanApplyCost = Super::CheckCost(Handle, ActorInfo, OptionalRelevantTags);

	if (const UMGRangedWeaponInstance* WeaponInstance = GetWeaponInstance())
	{
		bCanApplyCost = WeaponInstance->GetCurrentAmmo() - WeaponInstance->GetBulletsInOneShot() >= 0;
	}

	return bCanApplyCost;
}

void UMGWeaponFireGameplayAbility::ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	Super::ApplyCost(Handle, ActorInfo, ActivationInfo);

	if (ActorInfo->IsNetAuthority())
	{
		if (UMGRangedWeaponInstance* WeaponInstance = GetWeaponInstance())
		{
			const int CurrentAmmo = WeaponInstance->GetCurrentAmmo();
			const int BulletsInOneShot = WeaponInstance->GetBulletsInOneShot();
			WeaponInstance->SetCurrentAmmo(CurrentAmmo - BulletsInOneShot);
		}
	}
}
