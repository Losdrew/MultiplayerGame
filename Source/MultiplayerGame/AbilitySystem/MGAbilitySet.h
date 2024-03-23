// Copyright Artem Volovyi. All Rights Reserved.

#pragma once

#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "GameplayEffect.h"

#include "MGAbilitySet.generated.h"

class UAttributeSet;
class UGameplayEffect;
class UMGGameplayAbility;
class UMGAbilitySystemComponent;

/**
 * FMGAbilitySet_GameplayAbility
 *
 *	Data used by the ability set to grant gameplay abilities
 */
USTRUCT(BlueprintType)
struct FMGAbilitySet_GameplayAbility
{
	GENERATED_BODY()

	// Gameplay ability to grant
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UMGGameplayAbility> Ability = nullptr;

	// Level of ability to grant
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 AbilityLevel = 1;

	// Tag used to process input for the ability
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (Categories = "InputTag"))
	FGameplayTag InputTag;
};

/**
 * FMGAbilitySet_GameplayEffect
 *
 *	Data used by the ability set to grant gameplay effects
 */
USTRUCT(BlueprintType)
struct FMGAbilitySet_GameplayEffect
{
	GENERATED_BODY()

	// Gameplay effect to grant
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> GameplayEffect = nullptr;

	// Level of gameplay effect to grant
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float EffectLevel = 1.0f;
};

/**
 * FMGAbilitySet_AttributeSet
 *
 *	Data used by the ability set to grant attribute sets
 */
USTRUCT(BlueprintType)
struct FMGAbilitySet_AttributeSet
{
	GENERATED_BODY()

	// Gameplay effect to grant
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UAttributeSet> AttributeSet;
};

/**
 * FMGAbilitySet_GrantedHandles
 *
 *	Data used to store handles to what has been granted by the ability set
 */
USTRUCT(BlueprintType)
struct FMGAbilitySet_GrantedHandles
{
	GENERATED_BODY()

public:

	void AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle);
	void AddGameplayEffectHandle(const FActiveGameplayEffectHandle& Handle);
	void AddAttributeSet(UAttributeSet* Set);

	void TakeFromAbilitySystem(UMGAbilitySystemComponent* MGASC);

	bool operator==(const FMGAbilitySet_GrantedHandles& Other) const
	{
		return AbilitySpecHandles == Other.AbilitySpecHandles
			&& GameplayEffectHandles == Other.GameplayEffectHandles
			&& GrantedAttributeSets == Other.GrantedAttributeSets;
	}

protected:
	// Handles to the granted abilities
	UPROPERTY(BlueprintReadOnly)
	TArray<FGameplayAbilitySpecHandle> AbilitySpecHandles;

	// Handles to the granted gameplay effects
	UPROPERTY(BlueprintReadOnly)
	TArray<FActiveGameplayEffectHandle> GameplayEffectHandles;

	// Pointers to the granted attribute sets
	UPROPERTY(BlueprintReadOnly)
	TArray<TObjectPtr<UAttributeSet>> GrantedAttributeSets;
};

/**
 * UMGAbilitySet
 *
 *	Data asset used to grant gameplay abilities, gameplay attributes and gameplay effects
 */
UCLASS(Blueprintable, Const)
class UMGAbilitySet : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// Grants the ability set to the specified ability system component
	// The returned handles can be used later to take away anything that was granted
	void GiveToAbilitySystem(UMGAbilitySystemComponent* MGASC, FMGAbilitySet_GrantedHandles* OutGrantedHandles, UObject* SourceObject = nullptr) const;

protected:
	// Gameplay abilities to grant when this ability set is granted
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Abilities", meta=(TitleProperty=Ability))
	TArray<FMGAbilitySet_GameplayAbility> GrantedGameplayAbilities;

	// Gameplay effects to grant when this ability set is granted
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects", meta=(TitleProperty=GameplayEffect))
	TArray<FMGAbilitySet_GameplayEffect> GrantedGameplayEffects;

	// Attribute sets to grant when this ability set is granted
	UPROPERTY(EditDefaultsOnly, Category = "Attribute Sets", meta=(TitleProperty=AttributeSet))
	TArray<FMGAbilitySet_AttributeSet> GrantedAttributes;
};
