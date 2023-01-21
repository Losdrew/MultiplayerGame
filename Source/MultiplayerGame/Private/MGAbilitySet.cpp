// Copyright Artem Volovyi. All Rights Reserved.


#include "MGAbilitySet.h"

#include "MGAbilitySystemComponent.h"

void UMGAbilitySet::GiveToAbilitySystem(UMGAbilitySystemComponent* MGASC, UObject* SourceObject) const
{
	check(MGASC);

	if (!MGASC->IsOwnerActorAuthoritative())
	{
		// Must be authoritative to give or take ability sets.
		return;
	}

	// Grant the gameplay abilities.
	for (int32 AbilityIndex = 0; AbilityIndex < GrantedGameplayAbilities.Num(); ++AbilityIndex)
	{
		const FMGAbilitySet_GameplayAbility& AbilityToGrant = GrantedGameplayAbilities[AbilityIndex];

		if (!IsValid(AbilityToGrant.Ability))
		{
			continue;
		}

		UMGGameplayAbility* Ability = AbilityToGrant.Ability->GetDefaultObject<UMGGameplayAbility>();

		FGameplayAbilitySpec AbilitySpec(Ability, 1);
		AbilitySpec.SourceObject = SourceObject;
		AbilitySpec.DynamicAbilityTags.AddTag(AbilityToGrant.InputTag);

		MGASC->GiveAbility(AbilitySpec);
	}

	// Grant the attribute sets.
	for (int32 SetIndex = 0; SetIndex < GrantedAttributes.Num(); ++SetIndex)
	{
		const FMGAbilitySet_AttributeSet& SetToGrant = GrantedAttributes[SetIndex];

		if (!IsValid(SetToGrant.AttributeSet))
		{
			continue;
		}

		UAttributeSet* NewSet = NewObject<UAttributeSet>(MGASC->GetOwner(), SetToGrant.AttributeSet);
		MGASC->AddAttributeSetSubobject(NewSet);
	}
}
