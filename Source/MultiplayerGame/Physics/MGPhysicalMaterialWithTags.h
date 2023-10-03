// Copyright Artem Volovyi. All Rights Reserved.

#pragma once

#include "PhysicalMaterials/PhysicalMaterial.h"
#include "GameplayTagContainer.h"

#include "MGPhysicalMaterialWithTags.generated.h"

/**
 * UMGPhysicalMaterialWithTags
 *
 *	Physical material class that uses gameplay tags
 */
UCLASS()
class UMGPhysicalMaterialWithTags : public UPhysicalMaterial
{
	GENERATED_BODY()

public:
	// Tag container that can be used to specify details about a physical material
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=PhysicalProperties)
	FGameplayTagContainer Tags;

};
