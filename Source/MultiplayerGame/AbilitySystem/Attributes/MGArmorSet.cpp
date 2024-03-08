// Copyright Artem Volovyi. All Rights Reserved.


#include "MGArmorSet.h"

#include "GameplayEffectExtension.h"
#include "MGAbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"

UMGArmorSet::UMGArmorSet()
{
	Armor = 0.0f;
	MaxArmor = 100.0f;
}

void UMGArmorSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UMGArmorSet, Armor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMGArmorSet, MaxArmor, COND_None, REPNOTIFY_Always);
}

void UMGArmorSet::OnRep_Armor(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMGArmorSet, Armor, OldValue);
}

void UMGArmorSet::OnRep_MaxArmor(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMGArmorSet, MaxArmor, OldValue);
}

void UMGArmorSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetArmorAttribute())
	{
		// Clamp armor to be between 0 and MaxArmor
		SetArmor(FMath::Clamp(GetArmor(), 0.0f, GetMaxArmor()));
	}
}

void UMGArmorSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);

	ClampAttribute(Attribute, NewValue);
}

void UMGArmorSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	ClampAttribute(Attribute, NewValue);
}

void UMGArmorSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	if (Attribute == GetMaxArmorAttribute())
	{
		// Make sure current armor is not greater than the new max armor
		if (GetArmor() > NewValue)
		{
			UAbilitySystemComponent* AbilitySystemComponent = GetMGAbilitySystemComponent();
			check(AbilitySystemComponent);

			AbilitySystemComponent->ApplyModToAttribute(GetArmorAttribute(), EGameplayModOp::Override, NewValue);
		}
	}
}

void UMGArmorSet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
	if (Attribute == GetArmorAttribute())
	{
		// Do not allow armor to go negative or above max armor
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxArmor());
	}
	else if (Attribute == GetMaxArmorAttribute())
	{
		// Do not allow max armor to drop below 1
		NewValue = FMath::Max(NewValue, 1.0f);
	}
}

float UMGArmorSet::GetCurrentAbsorptionRate() const
{
	float CurrentArmor = GetArmor();
	float CurrentMaxArmor = GetMaxArmor();

	if (CurrentArmor == CurrentMaxArmor)
	{
		return 1.0f;
	}
	else if (CurrentArmor > (CurrentMaxArmor * 0.5) && CurrentArmor < CurrentMaxArmor)
	{
		return 0.75f;
	}
	else if (CurrentArmor <= (CurrentMaxArmor * 0.5) && CurrentArmor > 0)
	{
		return 0.5f;
	}

	return 0.0f;
}
