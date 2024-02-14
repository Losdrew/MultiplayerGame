// Copyright Artem Volovyi. All Rights Reserved.


#include "MGRangedWeaponInstance.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Engine/GameEngine.h"
#include "GameFramework/Pawn.h"
#include "GameplayTagContainer.h"
#include "MGCharacterMovementComponent.h"
#include "MGPhysicalMaterialWithTags.h"
#include "NativeGameplayTags.h"
#include "Net/UnrealNetwork.h"

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Status_Aiming, "Status.Aiming");


void UMGRangedWeaponInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UMGRangedWeaponInstance, CurrentAmmo, COND_OwnerOnly, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(UMGRangedWeaponInstance, TotalAmmo, COND_OwnerOnly, REPNOTIFY_OnChanged);
}

void UMGRangedWeaponInstance::Tick(float DeltaSeconds)
{
	APawn* Pawn = GetPawn();
	check(Pawn);

	const bool bMinSpread = UpdateSpread(DeltaSeconds);
	const bool bMinMultipliers = UpdateMultipliers(DeltaSeconds);
	
	bHasFirstShotAccuracy = bAllowFirstShotAccuracy && bMinMultipliers && bMinSpread;
}

void UMGRangedWeaponInstance::OnEquipped()
{
	Super::OnEquipped();

	// Start heat in the middle
	float MinHeatRange;
	float MaxHeatRange;
	ComputeHeatRange(MinHeatRange, MaxHeatRange);
	CurrentHeat = (MinHeatRange + MaxHeatRange) * 0.5f;

	// Derive spread
	CurrentSpreadAngle = HeatToSpreadCurve.GetRichCurveConst()->Eval(CurrentHeat);

	// Default the multipliers to 1x
	CurrentSpreadAngleMultiplier = 1.0f;
	StandingStillMultiplier = 1.0f;
	JumpFallMultiplier = 1.0f;
	SlidingMultiplier = 1.0f;
	WallrunningMultiplier = 1.0f;
}

void UMGRangedWeaponInstance::OnUnequipped()
{
	Super::OnUnequipped();
}

float UMGRangedWeaponInstance::GetDistanceAttenuation(float Distance) const
{
	const FRichCurve* Curve = DistanceDamageFalloff.GetRichCurveConst();
	return Curve->HasAnyData() ? Curve->Eval(Distance) : 1.0f;
}

float UMGRangedWeaponInstance::GetPhysicalMaterialAttenuation(const UPhysicalMaterial* PhysicalMaterial) const
{
	float Multiplier = 1.0f;

	if (const UMGPhysicalMaterialWithTags* PhysMatWithTags = Cast<const UMGPhysicalMaterialWithTags>(PhysicalMaterial))
	{
		for (const FGameplayTag MaterialTag : PhysMatWithTags->Tags)
		{
			if (const float* PTagMultiplier = MaterialDamageMultiplier.Find(MaterialTag))
			{
				Multiplier *= *PTagMultiplier;
			}
		}
	}

	return Multiplier;
}

void UMGRangedWeaponInstance::UpdateFiringTime()
{
	UWorld* World = GetWorld();
	check(World);
	LastFireTime = World->GetTimeSeconds();
}

void UMGRangedWeaponInstance::AddSpread()
{
	const float HeatPerShot = HeatToHeatPerShotCurve.GetRichCurveConst()->Eval(CurrentHeat);
	CurrentHeat = ClampHeat(CurrentHeat + HeatPerShot);

	// Map the heat to the spread angle
	CurrentSpreadAngle = HeatToSpreadCurve.GetRichCurveConst()->Eval(CurrentHeat);
}

void UMGRangedWeaponInstance::OnRep_CurrentAmmo()
{
	OnCurrentAmmoChanged.Broadcast(this, CurrentAmmo);
}

void UMGRangedWeaponInstance::OnRep_TotalAmmo()
{
	OnTotalAmmoChanged.Broadcast(this, TotalAmmo);
}

void UMGRangedWeaponInstance::ComputeSpreadRange(OUT float& MinSpread, OUT float& MaxSpread)
{
	HeatToSpreadCurve.GetRichCurveConst()->GetValueRange(MinSpread, MaxSpread);
}

void UMGRangedWeaponInstance::ComputeHeatRange(OUT float& MinHeat, OUT float& MaxHeat)
{
	float Min1;
	float Max1;
	HeatToHeatPerShotCurve.GetRichCurveConst()->GetTimeRange(Min1, Max1);

	float Min2;
	float Max2;
	HeatToCoolDownPerSecondCurve.GetRichCurveConst()->GetTimeRange(Min2, Max2);

	float Min3;
	float Max3;
	HeatToSpreadCurve.GetRichCurveConst()->GetTimeRange(Min3, Max3);

	MinHeat = FMath::Min(FMath::Min(Min1, Min2), Min3);
	MaxHeat = FMath::Max(FMath::Max(Max1, Max2), Max3);
}

float UMGRangedWeaponInstance::ClampHeat(float NewHeat)
{
	float MinHeat;
	float MaxHeat;
	ComputeHeatRange(MinHeat, MaxHeat);

	return FMath::Clamp(NewHeat, MinHeat, MaxHeat);
}

bool UMGRangedWeaponInstance::UpdateSpread(float DeltaSeconds)
{
	const float TimeSinceFired = GetWorld()->TimeSince(LastFireTime);

	if (TimeSinceFired > SpreadRecoveryCooldownDelay)
	{
		const float CooldownRate = HeatToCoolDownPerSecondCurve.GetRichCurveConst()->Eval(CurrentHeat);
		CurrentHeat = ClampHeat(CurrentHeat - CooldownRate * DeltaSeconds);
		CurrentSpreadAngle = HeatToSpreadCurve.GetRichCurveConst()->Eval(CurrentHeat);
	}

	float MinSpread;
	float MaxSpread;
	ComputeSpreadRange(MinSpread, MaxSpread);

	return FMath::IsNearlyEqual(CurrentSpreadAngle, MinSpread, KINDA_SMALL_NUMBER);
}

bool UMGRangedWeaponInstance::UpdateMultipliers(float DeltaSeconds)
{
	const float MultiplierNearlyEqualThreshold = 0.05f;

	APawn* Pawn = GetPawn();
	check(Pawn);
	UMGCharacterMovementComponent* CharacterMovement = Cast<UMGCharacterMovementComponent>(Pawn->GetMovementComponent());

	// See if we are standing still, and if so, smoothly apply the bonus
	const float PawnSpeed = Pawn->GetVelocity().Size();
	const float MovementTargetValue = FMath::GetMappedRangeValueClamped(
		/*InputRange=*/ FVector2D(StandingStillSpeedThreshold, StandingStillSpeedThreshold + StandingStillToMovingSpeedRange),
		/*OutputRange=*/ FVector2D(SpreadAngleMultiplier_StandingStill, 1.0f),
		/*Alpha=*/ PawnSpeed);
	StandingStillMultiplier = FMath::FInterpTo(StandingStillMultiplier, MovementTargetValue, DeltaSeconds, TransitionRate_StandingStill);
	const bool bStandingStillMultiplierAtMin = FMath::IsNearlyEqual(StandingStillMultiplier, SpreadAngleMultiplier_StandingStill, SpreadAngleMultiplier_StandingStill * 0.1f);

	// See if we are sliding, and if so, smoothly apply the bonus
	const bool bIsSliding = CharacterMovement && CharacterMovement->IsSliding();
	const float SlidingTargetValue = bIsSliding ? SpreadAngleMultiplier_Sliding : 1.0f;
	SlidingMultiplier = FMath::FInterpTo(SlidingMultiplier, SlidingTargetValue, DeltaSeconds, TransitionRate_Sliding);
	const bool bSlidingMultiplierIs1 = FMath::IsNearlyEqual(SlidingMultiplier, 1.0f, MultiplierNearlyEqualThreshold);

	// See if we are in the air (jumping/falling), and if so, smoothly apply the penalty
	const bool bIsJumpingOrFalling = CharacterMovement && CharacterMovement->IsFalling();
	const float JumpFallTargetValue = bIsJumpingOrFalling ? SpreadAngleMultiplier_JumpingOrFalling : 1.0f;
	JumpFallMultiplier = FMath::FInterpTo(JumpFallMultiplier, JumpFallTargetValue, DeltaSeconds, TransitionRate_JumpingOrFalling);
	const bool bJumpFallMultiplerIs1 = FMath::IsNearlyEqual(JumpFallMultiplier, 1.0f, MultiplierNearlyEqualThreshold);

	// See if we are wallrunning, and if so, smoothly apply the penalty
	const bool bIsWallrunning = CharacterMovement && CharacterMovement->IsWallrunning();
	const float WallrunningTargetValue = bIsWallrunning ? SpreadAngleMultiplier_Wallrunning : 1.0f;
	WallrunningMultiplier = FMath::FInterpTo(WallrunningMultiplier, WallrunningTargetValue, DeltaSeconds, TransitionRate_Wallrunning);
	const bool bWallrunningMultiplerIs1 = FMath::IsNearlyEqual(WallrunningMultiplier, 1.0f, MultiplierNearlyEqualThreshold);

	// Determine if we are aiming down sights, and apply the bonus
	float AimingAlpha = 0.0f;
	if (const UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Pawn))
	{
		AimingAlpha = ASC->HasMatchingGameplayTag(TAG_Status_Aiming) ? 1.0f : 0.0f;
	}
	const float AimingMultiplier = FMath::GetMappedRangeValueClamped(
		/*InputRange=*/ FVector2D(0.0f, 1.0f),
		/*OutputRange=*/ FVector2D(1.0f, SpreadAngleMultiplier_Aiming),
		/*Alpha=*/ AimingAlpha);
	const bool bAimingMultiplierAtTarget = FMath::IsNearlyEqual(AimingMultiplier, SpreadAngleMultiplier_Aiming, KINDA_SMALL_NUMBER);

	// Combine all the multipliers
	const float CombinedMultiplier = StandingStillMultiplier * SlidingMultiplier * JumpFallMultiplier * WallrunningMultiplier * AimingMultiplier;
	CurrentSpreadAngleMultiplier = CombinedMultiplier;

	// Need to handle these spread multipliers indicating we are not at min spread
	return bStandingStillMultiplierAtMin && bSlidingMultiplierIs1 && bJumpFallMultiplerIs1 && bWallrunningMultiplerIs1 && bAimingMultiplierAtTarget;
}
