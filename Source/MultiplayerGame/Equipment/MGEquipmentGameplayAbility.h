// Copyright Artem Volovyi. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MGEquipmentInstance.h"
#include "MGGameplayAbility.h"
#include "MGEquipmentGameplayAbility.generated.h"

/**
 * UMGEquipmentGameplayAbility
 *
 *	An ability granted by and associated with equipment instance
 */
UCLASS()
class UMGEquipmentGameplayAbility : public UMGGameplayAbility
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	UMGEquipmentInstance* GetAssociatedEquipment() const;
};
