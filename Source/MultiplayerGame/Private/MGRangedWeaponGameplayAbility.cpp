// Copyright Artem Volovyi. All Rights Reserved.


#include "MGRangedWeaponGameplayAbility.h"

#include "MGCharacter.h"
#include "MGRangedWeaponInstance.h"


UMGRangedWeaponInstance* UMGRangedWeaponGameplayAbility::GetWeaponInstance() const
{
	if (FGameplayAbilitySpec* Spec = GetCurrentAbilitySpec())
	{
		return Cast<UMGRangedWeaponInstance>(Spec->SourceObject.Get());
	}

	return nullptr;
}

bool UMGRangedWeaponGameplayAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                        const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
                                                        const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
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

bool UMGRangedWeaponGameplayAbility::CheckCost(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const
{
	bool bCanApplyCost = Super::CheckCost(Handle, ActorInfo, OptionalRelevantTags);

	if (const UMGRangedWeaponInstance* WeaponInstance = GetWeaponInstance())
	{
		bCanApplyCost = WeaponInstance->CurrentAmmoInClip - WeaponInstance->BulletsInOneShot >= 0;
	}

	return bCanApplyCost;
}

void UMGRangedWeaponGameplayAbility::ApplyCost(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	Super::ApplyCost(Handle, ActorInfo, ActivationInfo);

	if (ActorInfo->IsNetAuthority())
	{
		if (UMGRangedWeaponInstance* WeaponInstance = GetWeaponInstance())
		{
			WeaponInstance->CurrentAmmoInClip -= WeaponInstance->BulletsInOneShot;
		}
	}
}