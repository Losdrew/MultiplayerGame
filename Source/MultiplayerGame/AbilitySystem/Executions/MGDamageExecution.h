// Copyright Artem Volovyi. All Rights Reserved.

#pragma once

#include "GameplayEffectExecutionCalculation.h"

#include "MGDamageExecution.generated.h"

/**
 * UMGDamageExecution
 *
 *	Execution used by gameplay abilities to apply damage to health attributes
 */
UCLASS()
class UMGDamageExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:

	UMGDamageExecution();

protected:

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
