// Copyright Artem Volovyi. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MGAbilitySystemComponent.h"
#include "MGHealthSet.h"
#include "Components/ActorComponent.h"

#include "MGHealthComponent.generated.h"

class UMGHealthComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMGHealth_DeathEvent, AActor*, OwningActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMGHealth_OwnerKilled, AActor*, KillerActor, AActor*, KilledActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FMGHealth_AttributeChanged, UMGHealthComponent*, HealthComponent, float, OldValue, float, NewValue, AActor*, Instigator);

/**
 * EMGDeathState		
 *
 *	Defines current state of death
 */
UENUM(BlueprintType)
enum class EMGDeathState : uint8
{
	NotDead = 0,
	DeathStarted,
	DeathFinished
};

/**
 * UMGHealthComponent
 *
 *	An actor component used to handle anything related to health
 */
UCLASS()
class UMGHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMGHealthComponent();

	// Initialize the component using an ability system component
	UFUNCTION(BlueprintCallable)
	void InitializeWithAbilitySystem(UMGAbilitySystemComponent* InASC);

	// Uninitialize the component, clearing any references to the ability system
	UFUNCTION(BlueprintCallable)
	void UninitializeFromAbilitySystem();

	// Returns the current health value
	UFUNCTION(BlueprintCallable)
	float GetHealth() const;

	// Returns the normalized current health value
	UFUNCTION(BlueprintCallable)
	float GetHealthNormalized() const;

	// Returns the current maximum health value
	UFUNCTION(BlueprintCallable)
	float GetMaxHealth() const;

	UFUNCTION(BlueprintCallable)
	EMGDeathState GetDeathState() const { return DeathState; }

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Meta = (ExpandBoolAsExecs = "ReturnValue"))
	bool IsDeadOrDying() const { return DeathState > EMGDeathState::NotDead; }

	// Begins the death sequence for the owner
	virtual void StartDeath();

	// Ends the death sequence for the owner
	virtual void FinishDeath();

public:
	// Delegate fired when the health value has changed
	UPROPERTY(BlueprintAssignable)
	FMGHealth_AttributeChanged OnHealthChanged;

	// Delegate fired when the max health value has changed
	UPROPERTY(BlueprintAssignable)
	FMGHealth_AttributeChanged OnMaxHealthChanged;

	// Delegate fired when the owner is killed (needed for kill/death scoring)
	UPROPERTY(BlueprintAssignable)
	FMGHealth_OwnerKilled OnOwnerKilled;

	// Delegate fired when the death sequence has started
	UPROPERTY(BlueprintAssignable)
	FMGHealth_DeathEvent OnDeathStarted;

	// Delegate fired when the death sequence has finished
	UPROPERTY(BlueprintAssignable)
	FMGHealth_DeathEvent OnDeathFinished;

protected:

	virtual void OnUnregister() override;

	void ClearGameplayTags();

	virtual void HandleHealthChanged(const FOnAttributeChangeData& ChangeData);
	virtual void HandleMaxHealthChanged(const FOnAttributeChangeData& ChangeData);
	virtual void HandleOutOfHealth(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec& DamageEffectSpec, float DamageMagnitude);

	UFUNCTION()
	virtual void OnRep_DeathState(EMGDeathState OldDeathState);

protected:

	// Ability system used by this component.
	UPROPERTY()
	TObjectPtr<UMGAbilitySystemComponent> AbilitySystemComponent;

	// Health set used by this component.
	UPROPERTY()
	TObjectPtr<const UMGHealthSet> HealthSet;

	// Replicated state used to handle dying.
	UPROPERTY(ReplicatedUsing = OnRep_DeathState)
	EMGDeathState DeathState;
};
