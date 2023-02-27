// Copyright Artem Volovyi. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "MGAbilitySourceInterface.h"
#include "MGEquipmentInstance.h"
#include "Curves/CurveFloat.h"
#include "PhysicalMaterials/PhysicalMaterial.h"

#include "MGRangedWeaponInstance.generated.h"

/**
 * 
 */
UCLASS()
class UMGRangedWeaponInstance : public UMGEquipmentInstance, public IMGAbilitySourceInterface
{
	GENERATED_BODY()

public:
	virtual void PostInitProperties() override
	{
		Super::PostInitProperties();

		CurrentAmmoInClip = ClipSize;
		CurrentAmmoTotal = MaxAmmo;
	};

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;

	//~UMGEquipmentInstance interface
	virtual void OnEquipped();
	virtual void OnUnequipped();
	//~End of UMGEquipmentInstance interface

	//~IMGAbilitySourceInterface interface
	virtual float GetDistanceAttenuation(float Distance) const override;
	virtual float GetPhysicalMaterialAttenuation(const UPhysicalMaterial* PhysicalMaterial) const override;
	//~End of IMGAbilitySourceInterface interface

protected:
	// List of tags with damage multipliers associated with them
	// These tags are compared to the physical material of the thing that is hit
	// Multipliers are used in damage calculation
	UPROPERTY(EditAnywhere)
	TMap<FGameplayTag, float> MaterialDamageMultiplier;

	// A curve that maps the distance (in cm) to a multiplier on the base damage
	// If there is no data in this curve, then the weapon is assumed to have no falloff with distance
	UPROPERTY(EditAnywhere)
	FRuntimeFloatCurve DistanceDamageFalloff;

public:
	UPROPERTY(BlueprintReadOnly, Transient, Replicated)
	int CurrentAmmoInClip;

	UPROPERTY(BlueprintReadOnly, Transient, Replicated)
	int CurrentAmmoTotal;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	int ClipSize;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	int MaxAmmo;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	int BulletsInOneShot;
};
