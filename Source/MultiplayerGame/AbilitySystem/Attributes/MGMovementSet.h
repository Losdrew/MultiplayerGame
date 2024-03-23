// Copyright Artem Volovyi. All Rights Reserved.

#pragma once

#include "MGAttributeSet.h"

#include "MGMovementSet.generated.h"

/**
 * UMGMovementSet
 *
 *  Attribute set that defines attributes related to movement
 */
UCLASS()
class UMGMovementSet : public UMGAttributeSet
{
	GENERATED_BODY()
	
public:

	UMGMovementSet();

	ATTRIBUTE_ACCESSORS(UMGMovementSet, MaxJumpCount);

	UFUNCTION()
	void OnRep_MaxJumpCount(const FGameplayAttributeData& OldValue);

	// ~UAttributeSet interface
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
	// ~End of UAttributeSet interface

private:
	// The max amount of jumps attribute
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxJumpCount, Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxJumpCount;
};
