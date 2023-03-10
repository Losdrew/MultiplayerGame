// Copyright Artem Volovyi. All Rights Reserved.


#include "MGEquipmentGameplayAbility.h"

UMGEquipmentInstance* UMGEquipmentGameplayAbility::GetAssociatedEquipment() const
{
	if (const FGameplayAbilitySpec* Spec = GetCurrentAbilitySpec())
	{
		return Cast<UMGEquipmentInstance>(Spec->SourceObject.Get());
	}

	return nullptr;
}
