// Copyright Artem Volovyi. All Rights Reserved.


#include "MGDamageExecution.h"

#include "MGAbilitySourceInterface.h"
#include "MGHealthSet.h"
#include "MGArmorSet.h"
#include "MGCombatSet.h"

struct FDamageStatics
{
	FGameplayEffectAttributeCaptureDefinition BaseDamageDefinition;

	FDamageStatics()
	{
		BaseDamageDefinition = FGameplayEffectAttributeCaptureDefinition(UMGHealthSet::GetDamageAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
	}
};

static FDamageStatics& DamageStatics()
{
	static FDamageStatics Statics;
	return Statics;
}

UMGDamageExecution::UMGDamageExecution()
{
	RelevantAttributesToCapture.Add(DamageStatics().BaseDamageDefinition);
}

void UMGDamageExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
#if WITH_SERVER_CODE
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	FGameplayEffectContext* EffectContext = Spec.GetContext().Get();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvaluateParameters.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	float BaseDamage = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BaseDamageDefinition, EvaluateParameters, BaseDamage);

	const AActor* EffectCauser = EffectContext->GetEffectCauser();
	const FHitResult* HitActorResult = EffectContext->GetHitResult();

	AActor* HitActor = nullptr;
	FVector ImpactLocation = FVector::ZeroVector;
	FVector ImpactNormal = FVector::ZeroVector;
	FVector StartTrace = FVector::ZeroVector;
	FVector EndTrace = FVector::ZeroVector;

	// Calculation of hit actor, surface, and distance all rely on whether the calculation has a hit result or not.
	// Effects being added directly without having been targeted will not have a hit result, which must default
	// to some fallback information.
	if (HitActorResult)
	{
		const FHitResult& CurHitResult = *HitActorResult;
		HitActor = CurHitResult.HitObjectHandle.FetchActor();
		if (HitActor)
		{
			ImpactLocation = CurHitResult.ImpactPoint;
			ImpactNormal = CurHitResult.ImpactNormal;
			StartTrace = CurHitResult.TraceStart;
			EndTrace = CurHitResult.TraceEnd;
		}
	}

	// If there is no hit result or hit result not returning an actor, get targeted actor directly
	UAbilitySystemComponent* TargetAbilitySystemComponent = ExecutionParams.GetTargetAbilitySystemComponent();
	if (!HitActor)
	{
		HitActor = TargetAbilitySystemComponent ? TargetAbilitySystemComponent->GetAvatarActor_Direct() : nullptr;
		if (HitActor)
		{
			ImpactLocation = HitActor->GetActorLocation();
		}
	}

	// Determine distance between the effect's origin/causer and impact location
	double Distance = WORLD_MAX;
	if (EffectContext->HasOrigin())
	{
		Distance = FVector::Dist(EffectContext->GetOrigin(), ImpactLocation);
	}
	else if (EffectCauser)
	{
		Distance = FVector::Dist(EffectCauser->GetActorLocation(), ImpactLocation);
	}

	// Apply modifiers related to physical material hit and distance falloff
	float PhysicalMaterialAttenuation = 1.0f;
	float DistanceAttenuation = 1.0f;
	if (const IMGAbilitySourceInterface* AbilitySource = Cast<IMGAbilitySourceInterface>(Spec.GetContext().GetSourceObject()))
	{
		if (HitActorResult != nullptr)
		{
			if (const UPhysicalMaterial* PhysMaterial = HitActorResult->PhysMaterial.Get())
			{
				PhysicalMaterialAttenuation = AbilitySource->GetPhysicalMaterialAttenuation(PhysMaterial);
			}
		}

		DistanceAttenuation = AbilitySource->GetDistanceAttenuation(Distance);
	}

	float DamageMultiplier = 1.0f;
	if (const UMGCombatSet* CombatSet = ExecutionParams.GetSourceAbilitySystemComponent()->GetSet<UMGCombatSet>())
	{
		DamageMultiplier = CombatSet->GetDamageMultiplier();
	}

	float Armor = 0.0f;
	float ArmorAbsorptionRate = 0.0f;
	if (const UMGArmorSet* ArmorSet = TargetAbilitySystemComponent->GetSet<UMGArmorSet>())
	{
		Armor = ArmorSet->GetArmor();
		ArmorAbsorptionRate = ArmorSet->GetCurrentAbsorptionRate();
	}

	const float DamageDone = FMath::Max(BaseDamage * DamageMultiplier * DistanceAttenuation * PhysicalMaterialAttenuation, 0.0f);
	const float DamageToHealth = FMath::Min(DamageDone - DamageDone * ArmorAbsorptionRate, DamageDone);

	if (DamageDone > 0.0f)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UMGArmorSet::GetArmorAttribute(), EGameplayModOp::Additive, -DamageDone));
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UMGHealthSet::GetDamageAttribute(), EGameplayModOp::Additive, DamageToHealth));
	}
#endif WITH_SERVER_CODE
}
