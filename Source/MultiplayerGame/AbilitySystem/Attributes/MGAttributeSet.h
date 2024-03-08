// Copyright Artem Volovyi. All Rights Reserved.

#pragma once

#include "AttributeSet.h"
#include "AbilitySystemComponent.h"

#include "MGAttributeSet.generated.h"

class UMGAbilitySystemComponent;
struct FGameplayEffectSpec;

/**
 *	Defines a set of helper functions for accessing and initializing attributes.
 *
 *	For example, they will create the following functions:
 *		static FGameplayAttribute GetHealthAttribute();
 *		float GetHealth() const;
 *		void SetHealth(float NewVal);
 *		void InitHealth(float NewVal);
 */
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)


// Delegate used to broadcast attribute events.
DECLARE_MULTICAST_DELEGATE_FourParams(FMGAttributeEvent, AActor* /*EffectInstigator*/, AActor* /*EffectCauser*/, const FGameplayEffectSpec& /*EffectSpec*/, float /*EffectMagnitude*/);

/**
 * UMGAttributeSet
 *
 *	The base attribute set class used by this project
 */
UCLASS()
class UMGAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:

	UMGAbilitySystemComponent* GetMGAbilitySystemComponent() const;
};
