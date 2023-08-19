// Copyright Artem Volovyi. All Rights Reserved.


#include "MGGameplayAbility_WeaponFire.h"

bool UMGGameplayAbility_WeaponFire::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
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

bool UMGGameplayAbility_WeaponFire::CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const
{
	bool bCanApplyCost = Super::CheckCost(Handle, ActorInfo, OptionalRelevantTags);

	if (const UMGRangedWeaponInstance* WeaponInstance = GetWeaponInstance())
	{
		bCanApplyCost = WeaponInstance->GetCurrentAmmo() - WeaponInstance->GetBulletsInOneShot() >= 0;
	}

	return bCanApplyCost;
}

void UMGGameplayAbility_WeaponFire::ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
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
