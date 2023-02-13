// Copyright Artem Volovyi. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MGEquipmentInstance.h"

#include "MGRangedWeaponInstance.generated.h"

/**
 * 
 */
UCLASS()
class UMGRangedWeaponInstance : public UMGEquipmentInstance
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
