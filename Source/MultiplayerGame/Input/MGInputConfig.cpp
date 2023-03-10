// Copyright Artem Volovyi. All Rights Reserved.


#include "MGInputConfig.h"

const UInputAction* UMGInputConfig::FindNativeInputActionForTag(const FGameplayTag& InputTag) const
{
	for (const FMGInputAction& Action : NativeInputActions)
	{
		if (Action.InputAction && (Action.InputTag == InputTag))
		{
			return Action.InputAction;
		}
	}

	return nullptr;
}

const UInputAction* UMGInputConfig::FindAbilityInputActionForTag(const FGameplayTag& InputTag) const
{
	for (const FMGInputAction& Action : AbilityInputActions)
	{
		if (Action.InputAction && (Action.InputTag == InputTag))
		{
			return Action.InputAction;
		}
	}

	return nullptr;
}