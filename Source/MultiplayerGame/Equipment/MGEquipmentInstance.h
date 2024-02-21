// Copyright Artem Volovyi. All Rights Reserved.

#pragma once

#include "MGEquipmentInstance.generated.h"

class UAnimInstance;
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
	
	virtual void Tick(float DeltaSeconds);

	UFUNCTION(BlueprintPure, Category = "Equipment")
	APawn* GetPawn() const;

	UFUNCTION(BlueprintPure, Category = "Equipment")
	TArray<AActor*> GetSpawnedActorsThirdPerson() const { return SpawnedActorsThirdPerson; }

	UFUNCTION(BlueprintPure, Category = "Equipment")
	TArray<AActor*> GetSpawnedActorsFirstPerson() const { return SpawnedActorsFirstPerson; }

	virtual void SpawnEquipmentActors(const TArray<FMGEquipmentActorToSpawn>& ActorsToSpawn);
	void SetEquipmentActorsVisibility(bool bIsVisible);
	virtual void DestroyEquipmentActors();

	virtual void OnEquipped();
	virtual void OnUnequipped();

protected:

	UFUNCTION(BlueprintImplementableEvent, Category = "Equipment", meta = (DisplayName = "OnEquipped"))
	void K2_OnEquipped();

	UFUNCTION(BlueprintImplementableEvent, Category = "Equipment", meta = (DisplayName = "OnUnequipped"))
	void K2_OnUnequipped();

private:

	UFUNCTION()
	void OnRep_Instigator();

	UFUNCTION()
	void OnRep_SpawnedActorsThirdPerson();

	UFUNCTION()
	void OnRep_SpawnedActorsFirstPerson();

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Appearance")
	TObjectPtr<UTexture2D> EquipmentIcon;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	TSubclassOf<UAnimInstance> EquippedAnimLayerThirdPerson;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	TSubclassOf<UAnimInstance> EquippedAnimLayerFirstPerson;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	TSubclassOf<UAnimInstance> UnequippedAnimLayer;

private:

	UPROPERTY(ReplicatedUsing=OnRep_Instigator)
	TObjectPtr<UObject> Instigator;

	UPROPERTY(ReplicatedUsing=OnRep_SpawnedActorsThirdPerson)
	TArray<TObjectPtr<AActor>> SpawnedActorsThirdPerson;

	UPROPERTY(ReplicatedUsing = OnRep_SpawnedActorsFirstPerson)
	TArray<TObjectPtr<AActor>> SpawnedActorsFirstPerson;
};
