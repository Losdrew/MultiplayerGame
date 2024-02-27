// Copyright Artem Volovyi. All Rights Reserved.


#include "MGHealExecution.h"

#include "MGHealthSet.h"

struct FHealStatics
{
	FGameplayEffectAttributeCaptureDefinition BaseHealDefinition;

	FHealStatics()
	{
		BaseHealDefinition = FGameplayEffectAttributeCaptureDefinition(UMGHealthSet::GetHealingAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
	}
};

static FHealStatics& HealStatics()
{
	static FHealStatics Statics;
	return Statics;
}

UMGHealExecution::UMGHealExecution()
{
	RelevantAttributesToCapture.Add(HealStatics().BaseHealDefinition);
}

void UMGHealExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
#if WITH_SERVER_CODE
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	float BaseHeal = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(HealStatics().BaseHealDefinition, EvaluateParameters, BaseHeal);

	const float HealingDone = FMath::Max(0.0f, BaseHeal);

	if (HealingDone > 0.0f)
	{
		// Apply a healing modifier, this gets turned into + health on the target
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UMGHealthSet::GetHealingAttribute(), EGameplayModOp::Additive, HealingDone));
	}
#endif WITH_SERVER_CODE
}
