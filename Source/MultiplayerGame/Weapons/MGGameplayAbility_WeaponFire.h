// Copyright Artem Volovyi. All Rights Reserved.

#pragma once

#include "MGGameplayAbility_RangedWeapon.h"

#include "MGGameplayAbility_WeaponFire.generated.h"

/**
 * UMGGameplayAbility_WeaponFire
 *
 *  An ability used for firing a ranged weapon
 */
UCLASS()
class UMGGameplayAbility_WeaponFire : public UMGGameplayAbility_RangedWeapon
{
	GENERATED_BODY()

public:

	UMGGameplayAbility_WeaponFire();

	//~UGameplayAbility interface
	virtual bool CanActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual bool CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	//~End of UGameplayAbility interface

protected:
	struct FRangedWeaponFiringInput
	{
		// Start of the trace
		FVector StartTrace;

		// The direction of the trace if aim were perfect
		FVector AimDirection;

		// The weapon instance / source of weapon data
		UMGRangedWeaponInstance* WeaponData = nullptr;

		FRangedWeaponFiringInput()
			: StartTrace(ForceInitToZero)
			, AimDirection(ForceInitToZero)
		{
		}
	};

protected:

	void PerformLocalTargeting(OUT TArray<FHitResult>& OutHits);
	FTransform GetTargetingTransform(const APawn* SourcePawn);
	void TraceBulletsInOneShot(const FRangedWeaponFiringInput& InputData, OUT TArray<FHitResult>& OutHits);
	FVector GetRandomDirectionWithinCone(const FVector& BaseDirection, const float ConeHalfAngleRad, const float DistributionExponent);
	FHitResult WeaponTrace(const FVector& StartTrace, const FVector& EndTrace, OUT TArray<FHitResult>& OutHits) const;
	void AddAdditionalTraceIgnoreActors(FCollisionQueryParams& TraceParams) const;
	void OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& InData, FGameplayTag ApplicationTag);

	UFUNCTION(BlueprintCallable)
	void StartRangedWeaponTargeting();

	// Called when target data is ready
	UFUNCTION(BlueprintImplementableEvent)
	void OnRangedWeaponTargetDataReady(const FGameplayAbilityTargetDataHandle& TargetData);

private:
	FDelegateHandle OnTargetDataReadyCallbackDelegateHandle;
};
