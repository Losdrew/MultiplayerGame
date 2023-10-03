// Copyright Artem Volovyi. All Rights Reserved.

#pragma once

#include "MGEquipmentDefinition.generated.h"

class UMGAbilitySet;
class UMGEquipmentInstance;

/**
 * FMGEquipmentActorToSpawn
 *
 *	Holds information about actor to spawn from equipment
 */
USTRUCT()
struct FMGEquipmentActorToSpawn
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Equipment")
	TSubclassOf<AActor> ActorToSpawn;

	UPROPERTY(EditAnywhere, Category = "Equipment")
	FName AttachSocket;

	UPROPERTY(EditAnywhere, Category = "Equipment")
	FTransform AttachTransform;

	UPROPERTY(EditAnywhere, Category = "Equipment")
	bool bFirstPersonView;
};


/**
 * UMGEquipmentDefinition
 *
 *	Definition of a piece of equipment that can be applied to a pawn
 */
UCLASS(Blueprintable, Const, Abstract, BlueprintType)
class UMGEquipmentDefinition : public UObject
{
	GENERATED_BODY()

public:

	// Class to spawn
	UPROPERTY(EditDefaultsOnly, Category = "Equipment")
	TSubclassOf<UMGEquipmentInstance> InstanceType;

	// Gameplay ability sets to grant when this is equipped
	UPROPERTY(EditDefaultsOnly, Category = "Equipment")
	TArray<TObjectPtr<const UMGAbilitySet>> AbilitySetsToGrant;

	// Actors to spawn on the pawn when this is equipped
	UPROPERTY(EditDefaultsOnly, Category = "Equipment")
	TArray<FMGEquipmentActorToSpawn> ActorsToSpawn;
};
