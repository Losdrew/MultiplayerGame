// Copyright Artem Volovyi. All Rights Reserved.


#include "MGAttributeSet.h"

#include "MGAbilitySystemComponent.h"

UMGAbilitySystemComponent* UMGAttributeSet::GetMGAbilitySystemComponent() const
{
	return Cast<UMGAbilitySystemComponent>(GetOwningAbilitySystemComponent());
}
