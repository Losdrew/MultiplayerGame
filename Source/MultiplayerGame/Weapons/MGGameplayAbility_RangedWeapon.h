// Copyright Artem Volovyi. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MGRangedWeaponInstance.h"
#include "MGGameplayAbility_Equipment.h"
#include "MGGameplayAbility_RangedWeapon.generated.h"

/**
 * UMGGameplayAbility_Equipment
 *
 *	An ability granted by and associated with ranged weapon instance
 */
UCLASS()
class UMGGameplayAbility_RangedWeapon : public UMGGameplayAbility_Equipment
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	UMGRangedWeaponInstance* GetWeaponInstance() const;
};