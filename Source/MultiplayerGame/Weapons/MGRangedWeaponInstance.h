// Copyright Artem Volovyi. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "MGAbilitySourceInterface.h"
#include "MGEquipmentInstance.h"
#include "Curves/CurveFloat.h"

#include "MGRangedWeaponInstance.generated.h"

class UMGRangedWeaponInstance;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMGRangedWeapon_AttributeChanged, UMGRangedWeaponInstance*, WeaponInstance, int, NewValue);

/**
 * UMGRangedWeaponInstance
 *
 *	A piece of equipment representing a ranged weapon spawned and applied to a pawn
 */
UCLASS()
class UMGRangedWeaponInstance : public UMGEquipmentInstance, public IMGAbilitySourceInterface
{
	GENERATED_BODY()

public:
	virtual void PostInitProperties() override
	{
		Super::PostInitProperties();

		CurrentAmmo = ClipSize;
		TotalAmmo = MaxAmmo;
	};

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//~UMGEquipmentInstance interface
	virtual void OnEquipped();
	virtual void OnUnequipped();
	//~End of UMGEquipmentInstance interface

	//~IMGAbilitySourceInterface interface
	virtual float GetDistanceAttenuation(float Distance) const override;
	virtual float GetPhysicalMaterialAttenuation(const UPhysicalMaterial* PhysicalMaterial) const override;
	//~End of IMGAbilitySourceInterface interface

	UFUNCTION(BlueprintCallable)
	int GetCurrentAmmo() const
	{
		return CurrentAmmo;
	}

	UFUNCTION(BlueprintCallable)
	void SetCurrentAmmo(int NewValue)
	{
		const int OldValue = CurrentAmmo;
		CurrentAmmo = NewValue;
		OnCurrentAmmoChanged.Broadcast(this, CurrentAmmo);
	}

	UFUNCTION(BlueprintCallable)
	int GetTotalAmmo() const
	{
		return TotalAmmo;
	}

	UFUNCTION(BlueprintCallable)
	void SetTotalAmmo(int NewValue)
	{
		const int OldValue = TotalAmmo;
		TotalAmmo = NewValue;
		OnTotalAmmoChanged.Broadcast(this, TotalAmmo);
	}

	UFUNCTION(BlueprintCallable)
	int GetClipSize() const
	{
		return ClipSize;
	}

	UFUNCTION(BlueprintCallable)
	int GetMaxAmmo() const
	{
		return MaxAmmo;
	}

	UFUNCTION(BlueprintCallable)
	int GetBulletsInOneShot() const
	{
		return BulletsInOneShot;
	}

protected:

	UFUNCTION()
	void OnRep_CurrentAmmo();

	UFUNCTION()
	void OnRep_TotalAmmo();

public:
	// Delegate fired when current ammo is changed
	UPROPERTY(BlueprintAssignable)
	FMGRangedWeapon_AttributeChanged OnCurrentAmmoChanged;

	// Delegate fired when total ammo is changed
	UPROPERTY(BlueprintAssignable)
	FMGRangedWeapon_AttributeChanged OnTotalAmmoChanged;

public:

	UPROPERTY(EditDefaultsOnly, Category = "Appearance")
	TObjectPtr<UTexture2D> KillFeedIcon;

protected:
	// List of tags with damage multipliers associated with them
	// These tags are compared to the physical material of the thing that is hit
	// Multipliers are used in damage calculation
	UPROPERTY(EditAnywhere, Category = "Damage")
	TMap<FGameplayTag, float> MaterialDamageMultiplier;

	// A curve that maps the distance (in cm) to a multiplier on the base damage
	// If there is no data in this curve, then the weapon is assumed to have no falloff with distance
	UPROPERTY(EditAnywhere, Category = "Damage")
	FRuntimeFloatCurve DistanceDamageFalloff;

private:

	UPROPERTY(Transient, ReplicatedUsing=OnRep_CurrentAmmo)
	int CurrentAmmo;

	UPROPERTY(Transient, ReplicatedUsing=OnRep_TotalAmmo)
	int TotalAmmo;

	UPROPERTY(EditAnywhere, Category = "Ammo")
	int ClipSize;

	UPROPERTY(EditAnywhere, Category = "Ammo")
	int MaxAmmo;

	UPROPERTY(EditAnywhere, Category = "Ammo")
	int BulletsInOneShot;
};
