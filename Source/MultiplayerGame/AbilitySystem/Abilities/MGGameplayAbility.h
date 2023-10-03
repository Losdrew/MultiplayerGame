// Copyright Artem Volovyi. All Rights Reserved.

#pragma once

#include "Abilities/GameplayAbility.h"

#include "MGGameplayAbility.generated.h"

class AMGCharacter;
class AMGPlayerController;

/**
 * EMGAbilityActivationPolicy
 *
 *	Defines how an ability is meant to activate
 */
UENUM(BlueprintType)
enum class EMGAbilityActivationPolicy : uint8
{
	// Try to activate the ability when the input is triggered
	OnInputTriggered,

	// Continually try to activate the ability while the input is active
	WhileInputActive,

	// Try to activate the ability when an avatar is assigned
	OnSpawn
};

/**
 * UMGGameplayAbility
 *
 *	The base gameplay ability class used by this project
 */
UCLASS(Abstract, HideCategories = Input, Meta = (ShortTooltip = "The base gameplay ability class used by this project"))
class UMGGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

	friend class UMGAbilitySystemComponent;

public:

	UFUNCTION(BlueprintCallable)
	APlayerController* GetPlayerControllerFromActorInfo() const;

	UFUNCTION(BlueprintCallable)
	AMGPlayerController* GetMGPlayerControllerFromActorInfo() const;

	UFUNCTION(BlueprintCallable)
	AMGCharacter* GetMGCharacterFromActorInfo() const;

	void TryActivateAbilityOnSpawn(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) const;

protected:
	// ~UGameplayAbility interface
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	// ~End of UGameplayAbility interface

	EMGAbilityActivationPolicy GetActivationPolicy() const { return ActivationPolicy; }

	void OnPawnAvatarSet();

	// Called when this ability is granted to the ability system component
	UFUNCTION(BlueprintImplementableEvent, Category = "Ability", DisplayName = "OnAbilityAdded")
	void K2_OnAbilityAdded();

	// Called when the ability system is initialized with a pawn avatar
	UFUNCTION(BlueprintImplementableEvent, Category = "Ability", DisplayName = "OnPawnAvatarSet")
	void K2_OnPawnAvatarSet();

protected:
	// Defines how this ability is meant to activate
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability Activation")
	EMGAbilityActivationPolicy ActivationPolicy;
};
