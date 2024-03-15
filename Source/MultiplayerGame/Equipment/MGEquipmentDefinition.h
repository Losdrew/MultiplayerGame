// Copyright Artem Volovyi. All Rights Reserved.

#pragma once

#include "MGEquipmentDefinition.generated.h"

class UStaticMesh;
class UMGAbilitySet;
class UMGEquipmentInstance;

/**
 * FMGEquipmentActorToSpawn
 *
 *	Holds information about actor to spawn from equipment
 *  Gets attached to the pawn when equipment is equipped
 */
USTRUCT(BlueprintType)
struct FMGEquipmentActorToSpawn
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment")
	TSubclassOf<AActor> ActorToSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment")
	FName AttachSocket;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment")
	FTransform AttachTransform;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment")
	bool bFirstPersonView;
};

/**
 * FMGPickupActor
 *
 *	Holds information about pickup actor to spawn from equipment
 */
USTRUCT(BlueprintType)
struct FMGPickupActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment")
	TSubclassOf<AActor> PickupActor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment")
	TObjectPtr<UStaticMesh> PickupMesh;
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
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment")
	TSubclassOf<UMGEquipmentInstance> InstanceType;

	// Gameplay ability sets to grant when this is equipped
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment")
	TArray<TObjectPtr<const UMGAbilitySet>> AbilitySetsToGrant;

	// Actors to spawn on the pawn when this is equipped
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment")
	TArray<FMGEquipmentActorToSpawn> ActorsToSpawn;

	// MIGHT WANT TO CHANGE! Actor to spawn as a pickup when this is dropped
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment")
	FMGPickupActor PickupActor;
};
