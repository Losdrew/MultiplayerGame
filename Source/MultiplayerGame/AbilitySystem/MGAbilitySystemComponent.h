// Copyright Artem Volovyi. All Rights Reserved.

#pragma once

#include "AbilitySystemComponent.h"

#include "MGAbilitySystemComponent.generated.h"

/**
 * UMGAbilitySystemComponent
 *
 *	The base ability system component class used by this project
 */
UCLASS()
class UMGAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:

	void AbilityInputTagPressed(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);
	void ProcessAbilityInput(float DeltaTime, bool bGamePaused);

	void ClearAbilityInput();

	virtual void InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor) override;

protected:

	virtual void AbilitySpecInputPressed(FGameplayAbilitySpec& Spec) override;
	virtual void AbilitySpecInputReleased(FGameplayAbilitySpec& Spec) override;

	void TryActivateAbilitiesOnSpawn();

protected:
	// Handles to abilities that had their input pressed this frame
	TArray<FGameplayAbilitySpecHandle> InputPressedSpecHandles;

	// Handles to abilities that had their input released this frame
	TArray<FGameplayAbilitySpecHandle> InputReleasedSpecHandles;

	// Handles to abilities that have their input held
	TArray<FGameplayAbilitySpecHandle> InputHeldSpecHandles;

	// Handles to abilities that were triggered by input
	TArray<FGameplayAbilitySpecHandle> InputTriggeredSpecHandles;
};
