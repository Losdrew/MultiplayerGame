// Copyright Artem Volovyi. All Rights Reserved.

#pragma once

#include "MGAttributeSet.h"

#include "MGCombatSet.generated.h"

/**
 * UMGCombatSet
 *
 *  Attribute set that defines attributes related to combat
 */
UCLASS()
class UMGCombatSet : public UMGAttributeSet
{
	GENERATED_BODY()
	
public:

	UMGCombatSet();

	ATTRIBUTE_ACCESSORS(UMGCombatSet, DamageMultiplier);
	ATTRIBUTE_ACCESSORS(UMGCombatSet, FireRateMultiplier);
	ATTRIBUTE_ACCESSORS(UMGCombatSet, ReloadSpeedMultiplier);

protected:

	UFUNCTION()
	void OnRep_DamageMultiplier(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_FireRateMultiplier(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_ReloadSpeedMultiplier(const FGameplayAttributeData& OldValue);

private:
	// The base amount of damage to apply in the damage execution.
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_DamageMultiplier, Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData DamageMultiplier;

	// The fire rate multiplier attribute
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_FireRateMultiplier, Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData FireRateMultiplier;

	// The reload speed multiplier attribute
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ReloadSpeedMultiplier, Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData ReloadSpeedMultiplier;
};
