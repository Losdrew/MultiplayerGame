// Copyright Artem Volovyi. All Rights Reserved.

#pragma once

#include "MGEquipmentInstance.h"
#include "MGAbilitySourceInterface.h"
#include "GameplayTagContainer.h"

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
	
	virtual void Tick(float DeltaSeconds) override;

	//~UMGEquipmentInstance interface
	virtual void OnEquipped();
	virtual void OnUnequipped();
	//~End of UMGEquipmentInstance interface

	//~IMGAbilitySourceInterface interface
	virtual float GetDistanceAttenuation(float Distance) const override;
	virtual float GetPhysicalMaterialAttenuation(const UPhysicalMaterial* PhysicalMaterial) const override;
	//~End of IMGAbilitySourceInterface interface

	UFUNCTION(BlueprintCallable)
	void UpdateFiringTime();

	void AddSpread();

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

	/** Returns the current spread angle (in degrees, diametrical) */
	float GetCalculatedSpreadAngle() const
	{
		return CurrentSpreadAngle;
	}

	float GetCalculatedSpreadAngleMultiplier() const
	{
		return bHasFirstShotAccuracy ? 0.0f : CurrentSpreadAngleMultiplier;
	}

	bool HasFirstShotAccuracy() const
	{
		return bHasFirstShotAccuracy;
	}

	float GetSpreadExponent() const
	{
		return SpreadExponent;
	}

	float GetMaxDamageRange() const
	{
		return MaxDamageRange;
	}

protected:

	UFUNCTION()
	void OnRep_CurrentAmmo();

	UFUNCTION()
	void OnRep_TotalAmmo();

private:

	void ComputeSpreadRange(OUT float& MinSpread, OUT float& MaxSpread);
	void ComputeHeatRange(OUT float& MinHeat, OUT float& MaxHeat);

	float ClampHeat(float NewHeat);

	// Updates the spread and returns true if the spread is at minimum
	bool UpdateSpread(float DeltaSeconds);

	// Updates the multipliers and returns true if they are at minimum
	bool UpdateMultipliers(float DeltaSeconds);

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
	// The maximum number of bullets in a single clip
	UPROPERTY(EditAnywhere, Category = "Ammo")
	int32 ClipSize = 30;

	// The maximum number of bullets that can be carried for this weapon
	UPROPERTY(EditAnywhere, Category = "Ammo")
	int32 MaxAmmo = 120;
	
	// Number of bullets fired in a single shot
	UPROPERTY(EditAnywhere, Category = "Ammo")
	int32 BulletsInOneShot = 1;

	// Spread exponent, affects how tightly shots will cluster around the center line
	// when the weapon has spread (non-perfect accuracy). Higher values will cause shots
	// to be closer to the center (default is 1.0 which means uniformly within the spread range)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = 0.1), Category = "Spread|Fire Params")
	float SpreadExponent = 1.0f;

	// A curve that maps the heat to the spread angle
	// The X range of this curve typically sets the min/max heat range of the weapon
	// The Y range of this curve is used to define the min and maximum spread angle
	UPROPERTY(EditAnywhere, Category = "Spread|Fire Params")
	FRuntimeFloatCurve HeatToSpreadCurve;

	// A curve that maps the current heat to the amount a single shot will further 'heat up'
	// This is typically a flat curve with a single data point indicating how much heat a shot adds,
	// but can be other shapes to do things like punish overheating by adding progressively more heat.
	UPROPERTY(EditAnywhere, Category = "Spread|Fire Params")
	FRuntimeFloatCurve HeatToHeatPerShotCurve;

	// A curve that maps the current heat to the heat cooldown rate per second
	// This is typically a flat curve with a single data point indicating how fast the heat
	// wears off, but can be other shapes to do things like punish overheating by slowing down
	// recovery at high heat.
	UPROPERTY(EditAnywhere, Category = "Spread|Fire Params")
	FRuntimeFloatCurve HeatToCoolDownPerSecondCurve;

	// Time since firing before spread cooldown recovery begins (in seconds)
	UPROPERTY(EditAnywhere, Category = "Spread|Fire Params", meta = (ForceUnits = s))
	float SpreadRecoveryCooldownDelay = 0.0f;

	// Should the weapon have perfect accuracy when both player and weapon spread are at their minimum value
	UPROPERTY(EditAnywhere, Category = "Spread|Fire Params")
	bool bAllowFirstShotAccuracy = false;

	// Multiplier when standing still or moving very slowly
	// (starts to fade out at StandingStillSpeedThreshold, and is gone completely by StandingStillSpeedThreshold + StandingStillToMovingSpeedRange)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spread|Player Params", meta = (ForceUnits = x))
	float SpreadAngleMultiplier_StandingStill = 1.0f;

	// Rate at which we transition to/from the standing still accuracy (higher values are faster, though zero is instant; @see FInterpTo)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spread|Player Params")
	float TransitionRate_StandingStill = 5.0f;

	// Speeds at or below this are considered standing still
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spread|Player Params", meta = (ForceUnits = "cm/s"))
	float StandingStillSpeedThreshold = 80.0f;

	// Speeds no more than this above StandingStillSpeedThreshold are used to feather down the standing still bonus until it's back to 1.0
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spread|Player Params", meta = (ForceUnits = "cm/s"))
	float StandingStillToMovingSpeedRange = 20.0f;

	// Multiplier when sliding, smoothly blended to based on TransitionRate_Sliding
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spread|Player Params", meta = (ForceUnits = x))
	float SpreadAngleMultiplier_Sliding = 1.0f;

	// Rate at which we transition to/from the sliding accuracy (higher values are faster, though zero is instant; @see FInterpTo)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spread|Player Params")
	float TransitionRate_Sliding = 5.0f;

	// Spread multiplier while jumping/falling, smoothly blended to based on TransitionRate_JumpingOrFalling
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spread|Player Params", meta = (ForceUnits = x))
	float SpreadAngleMultiplier_JumpingOrFalling = 1.0f;

	// Rate at which we transition to/from the jumping/falling accuracy (higher values are faster, though zero is instant; @see FInterpTo)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spread|Player Params")
	float TransitionRate_JumpingOrFalling = 5.0f;

	// Spread multiplier while wallrunning, smoothly blended to based on TransitionRate_Wallrunning
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spread|Player Params", meta = (ForceUnits = x))
	float SpreadAngleMultiplier_Wallrunning = 1.0f;

	// Rate at which we transition to/from the wallrunning accuracy (higher values are faster, though zero is instant; @see FInterpTo)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spread|Player Params")
	float TransitionRate_Wallrunning = 5.0f;

	// The maximum distance at which this weapon can deal damage
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Config", meta = (ForceUnits = cm))
	float MaxDamageRange = 25000.0f;

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
	// The amount of ammo currently in the clip
	UPROPERTY(Transient, ReplicatedUsing=OnRep_CurrentAmmo)
	int32 CurrentAmmo;

	// Total amount of ammo currently available
	UPROPERTY(Transient, ReplicatedUsing=OnRep_TotalAmmo)
	int32 TotalAmmo;

private:
	// Time since this weapon was last fired (relative to world time)
	double LastFireTime = 0.0;

	// The current heat
	float CurrentHeat = 0.0f;

	// The current spread angle (in degrees, diametrical)
	float CurrentSpreadAngle = 0.0f;

	// Do we currently have first shot accuracy?
	bool bHasFirstShotAccuracy = false;

	// The current *combined* spread angle multiplier
	float CurrentSpreadAngleMultiplier = 1.0f;

	// The current standing still multiplier
	float StandingStillMultiplier = 1.0f;

	// The current jumping/falling multiplier
	float JumpFallMultiplier = 1.0f;

	// The current crouching multiplier
	float SlidingMultiplier = 1.0f;

	// The current wallrunning multiplier
	float WallrunningMultiplier = 1.0f;
};
