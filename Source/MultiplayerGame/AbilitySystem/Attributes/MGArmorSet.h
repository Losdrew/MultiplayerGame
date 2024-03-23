// Copyright Artem Volovyi. All Rights Reserved.

#pragma once

#include "MGAttributeSet.h"

#include "MGArmorSet.generated.h"

/**
 * UMGArmorSet
 *
 *	Attribute set that defines attributes related to armor 
 */
UCLASS()
class UMGArmorSet : public UMGAttributeSet
{
	GENERATED_BODY()
	
public:

	UMGArmorSet();

	ATTRIBUTE_ACCESSORS(UMGArmorSet, Armor);
	ATTRIBUTE_ACCESSORS(UMGArmorSet, MaxArmor);

	UFUNCTION()
	void OnRep_Armor(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MaxArmor(const FGameplayAttributeData& OldValue);

	// ~UAttributeSet interface
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
	// ~End of UAttributeSet interface

	void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;

	float GetCurrentAbsorptionRate() const;

private:
	// The current armor attribute
	// The armor will be capped by the max armor attribute
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Armor, Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Armor;

	// The current max armor attribute
	// Max armor is an attribute since gameplay effects can modify it
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxArmor, Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxArmor;
};
