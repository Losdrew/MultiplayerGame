// Copyright Artem Volovyi. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MGRangedWeaponInstance.h"
#include "MGEquipmentGameplayAbility.h"
#include "MGRangedWeaponGameplayAbility.generated.h"

/**
 *  UMGEquipmentGameplayAbility
 *
 *	An ability granted by and associated with ranged weapon instance
 */
UCLASS()
class UMGRangedWeaponGameplayAbility : public UMGEquipmentGameplayAbility
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	UMGRangedWeaponInstance* GetWeaponInstance() const;
};