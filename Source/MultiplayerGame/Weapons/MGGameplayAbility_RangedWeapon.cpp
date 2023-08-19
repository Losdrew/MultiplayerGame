// Copyright Artem Volovyi. All Rights Reserved.


#include "MGGameplayAbility_RangedWeapon.h"

#include "MGRangedWeaponInstance.h"

UMGRangedWeaponInstance* UMGGameplayAbility_RangedWeapon::GetWeaponInstance() const
{
	return Cast<UMGRangedWeaponInstance>(GetAssociatedEquipment());
}
