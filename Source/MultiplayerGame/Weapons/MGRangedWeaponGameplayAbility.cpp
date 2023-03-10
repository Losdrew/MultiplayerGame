// Copyright Artem Volovyi. All Rights Reserved.


#include "MGRangedWeaponGameplayAbility.h"

#include "MGRangedWeaponInstance.h"

UMGRangedWeaponInstance* UMGRangedWeaponGameplayAbility::GetWeaponInstance() const
{
	return Cast<UMGRangedWeaponInstance>(GetAssociatedEquipment());
}