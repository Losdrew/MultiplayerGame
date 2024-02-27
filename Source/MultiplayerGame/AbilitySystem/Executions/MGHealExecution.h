// Copyright Artem Volovyi. All Rights Reserved.

#pragma once

#include "GameplayEffectExecutionCalculation.h"

#include "MGHealExecution.generated.h"

/**
 * UMGHealExecution
 *
 *	Execution used by health pickups to apply healing
 */
UCLASS()
class UMGHealExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
	
public:

	UMGHealExecution();

protected:

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
