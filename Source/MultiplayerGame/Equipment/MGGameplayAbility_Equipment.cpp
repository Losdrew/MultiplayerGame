// Copyright Artem Volovyi. All Rights Reserved.


#include "MGGameplayAbility_Equipment.h"

UMGEquipmentInstance* UMGGameplayAbility_Equipment::GetAssociatedEquipment() const
{
	if (const FGameplayAbilitySpec* Spec = GetCurrentAbilitySpec())
	{
		return Cast<UMGEquipmentInstance>(Spec->SourceObject.Get());
	}

	return nullptr;
}
