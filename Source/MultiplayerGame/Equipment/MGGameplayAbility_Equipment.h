// Copyright Artem Volovyi. All Rights Reserved.

#pragma once

#include "MGGameplayAbility.h"

#include "MGGameplayAbility_Equipment.generated.h"

class UMGEquipmentInstance;

/**
 * UMGGameplayAbility_Equipment
 *
 *	An ability granted by and associated with equipment instance
 */
UCLASS()
class UMGGameplayAbility_Equipment : public UMGGameplayAbility
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	UMGEquipmentInstance* GetAssociatedEquipment() const;
};
