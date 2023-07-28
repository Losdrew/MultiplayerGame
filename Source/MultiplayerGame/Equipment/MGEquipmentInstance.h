// Copyright Artem Volovyi. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "MGEquipmentInstance.generated.h"

struct FMGEquipmentActorToSpawn;

/**
 * UMGEquipmentInstance
 *
 *	A piece of equipment spawned and applied to a pawn
 */
UCLASS(BlueprintType, Blueprintable)
class UMGEquipmentInstance : public UObject
{
	GENERATED_BODY()

public:
	//~UObject interface
	virtual bool IsSupportedForNetworking() const override { return true; }
	virtual UWorld* GetWorld() const override final;
	//~End of UObject interface

	UFUNCTION(BlueprintPure, Category = "Equipment")
	APawn* GetPawn() const;

	UFUNCTION(BlueprintPure, Category = "Equipment")
	TArray<AActor*> GetSpawnedActorsThirdPerson() const { return SpawnedActorsThirdPerson; }

	UFUNCTION(BlueprintPure, Category = "Equipment")
	TArray<AActor*> GetSpawnedActorsFirstPerson() const { return SpawnedActorsFirstPerson; }

	virtual void SpawnEquipmentActors(const TArray<FMGEquipmentActorToSpawn>& ActorsToSpawn);
	virtual void DestroyEquipmentActors();

	virtual void OnEquipped();
	virtual void OnUnequipped();

private:

	UFUNCTION()
	void OnRep_Instigator();

public:

	UPROPERTY(EditDefaultsOnly, Category = "Appearance")
	TObjectPtr<UTexture2D> EquipmentIcon;

private:

	UPROPERTY(ReplicatedUsing=OnRep_Instigator)
	TObjectPtr<UObject> Instigator;

	UPROPERTY(Replicated)
	TArray<TObjectPtr<AActor>> SpawnedActorsThirdPerson;

	UPROPERTY(Replicated)
	TArray<TObjectPtr<AActor>> SpawnedActorsFirstPerson;
};
