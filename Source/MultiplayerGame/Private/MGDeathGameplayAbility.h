// Copyright Artem Volovyi. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MGGameplayAbility.h"
#include "MGDeathGameplayAbility.generated.h"

/**
 * UMGDeathGameplayAbility
 *
 *	Gameplay ability used for handling death.
 */
UCLASS(Abstract)
class UMGDeathGameplayAbility : public UMGGameplayAbility
{
	GENERATED_BODY()

public:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UFUNCTION(BlueprintCallable)
	void StartDeath();

	UFUNCTION(BlueprintCallable)
	void FinishDeath();
};
