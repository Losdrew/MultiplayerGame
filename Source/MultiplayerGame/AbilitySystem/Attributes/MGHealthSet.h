// Copyright Artem Volovyi. All Rights Reserved.

#pragma once

#include "MGAttributeSet.h"

#include "MGHealthSet.generated.h"

/**
 * UMGHealthSet
 *
 *	Attribute set that defines attributes related to health and taking damage
 */
UCLASS()
class UMGHealthSet : public UMGAttributeSet
{
	GENERATED_BODY()

public:

	UMGHealthSet();

	ATTRIBUTE_ACCESSORS(UMGHealthSet, Health);
	ATTRIBUTE_ACCESSORS(UMGHealthSet, MaxHealth);
	ATTRIBUTE_ACCESSORS(UMGHealthSet, Healing);
	ATTRIBUTE_ACCESSORS(UMGHealthSet, Damage);

	// Delegate to broadcast when receiving damage
	mutable FMGAttributeEvent OnReceiveDamage;

	// Delegate to broadcast when the health attribute reaches zero
	mutable FMGAttributeEvent OnOutOfHealth;

	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldValue);

	// ~UAttributeSet interface
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
	// ~End of UAttributeSet interface

	void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;

private:
	// The current health attribute
	// The health will be capped by the max health attribute
	// Health is hidden from modifiers so only executions can modify it
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Meta = (HideFromModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData Health;

	// The current max health attribute
	// Max health is an attribute since gameplay effects can modify it
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxHealth;

	// Used to track when the health reaches 0.
	bool bOutOfHealth;

private:
	// Incoming healing. This is mapped directly to +Health
	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Healing;

	// Incoming damage. This is mapped directly to -Health
	UPROPERTY(BlueprintReadOnly, Meta = (HideFromModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData Damage;
};
