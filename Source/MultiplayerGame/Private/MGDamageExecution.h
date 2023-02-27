// Copyright Artem Volovyi. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "MGDamageExecution.generated.h"

/**
 * 
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
