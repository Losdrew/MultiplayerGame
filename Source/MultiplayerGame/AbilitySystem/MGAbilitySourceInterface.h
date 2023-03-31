// Copyright Artem Volovyi. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MGAbilitySourceInterface.generated.h"

// This class does not need to be modified
UINTERFACE(MinimalAPI)
class UMGAbilitySourceInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * IMGAbilitySourceInterface
 *
 *	Base interface for anything acting as an ability calculation source
 */
class IMGAbilitySourceInterface
{
	GENERATED_BODY()

public:
	// Compute multiplier for effect falloff with distance
	virtual float GetDistanceAttenuation(float Distance) const = 0;

	// Compute multiplier for effect affected by physical material
	// Also used for multiplying the effect depending on the part of the body that is hit
	virtual float GetPhysicalMaterialAttenuation(const UPhysicalMaterial* PhysicalMaterial) const = 0;
};
