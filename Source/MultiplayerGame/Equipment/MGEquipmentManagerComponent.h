// Copyright Artem Volovyi. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MGEquipmentDefinition.h"

#include "MGEquipmentManagerComponent.generated.h"

struct FMGEquipmentList;
class UMGEquipmentManagerComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMGEquipment_EquippedChanged, UMGEquipmentManagerComponent*, EquipmentManagerComponent, UMGEquipmentInstance*, EquipmentInstance);

/**
 * FMGAppliedEquipmentEntry
 *
 *	A single piece of applied equipment
 */
USTRUCT(BlueprintType)
struct FMGAppliedEquipmentEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

	FMGAppliedEquipmentEntry() {}

private:

	friend FMGEquipmentList;
	friend UMGEquipmentManagerComponent;

	// The equipment class that got equipped
	UPROPERTY()
	TSubclassOf<UMGEquipmentDefinition> EquipmentDefinition;

	UPROPERTY()
	TObjectPtr<UMGEquipmentInstance> Instance = nullptr;

	// Authority-only list of granted handles
	UPROPERTY(NotReplicated)
	FMGAbilitySet_GrantedHandles GrantedHandles;
};


/**
 * FMGEquipmentList
 *
 *	List of applied equipment
 */
USTRUCT(BlueprintType)
struct FMGEquipmentList : public FFastArraySerializer
{
	GENERATED_BODY()

	FMGEquipmentList() : OwnerComponent(nullptr) {}

	FMGEquipmentList(UActorComponent* InOwnerComponent) : OwnerComponent(InOwnerComponent) {}

public:
	//~FFastArraySerializer contract
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
	//~End of FFastArraySerializer contract

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FastArrayDeltaSerialize<FMGAppliedEquipmentEntry, FMGEquipmentList>(Items, DeltaParms, *this);
	}

	UMGEquipmentInstance* AddEntry(TSubclassOf<UMGEquipmentDefinition> EquipmentDefinition);
	void RemoveEntry(UMGEquipmentInstance* Instance);

private:

	friend UMGEquipmentManagerComponent;

	UMGAbilitySystemComponent* GetAbilitySystemComponent() const;

private:
	// Replicated list of equipment entries
	UPROPERTY()
	TArray<FMGAppliedEquipmentEntry> Items;

	UPROPERTY(NotReplicated)
	TObjectPtr<UActorComponent> OwnerComponent;
};

template<>
struct TStructOpsTypeTraits<FMGEquipmentList> : public TStructOpsTypeTraitsBase2<FMGEquipmentList>
{
	enum { WithNetDeltaSerializer = true };
};


/**
 * UMGEquipmentManagerComponent
 *
 *	Component that manages equipment applied to a pawn
 */
UCLASS()
class UMGEquipmentManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UMGEquipmentManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	UMGEquipmentInstance* EquipItem(TSubclassOf<UMGEquipmentDefinition> EquipmentClass);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void UnequipItem(UMGEquipmentInstance* ItemInstance);

	//~UObject interface
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	//~End of UObject interface

	//~UActorComponent interface
	virtual void InitializeComponent() override;
	virtual void UninitializeComponent() override;
	virtual void ReadyForReplication() override;
	//~End of UActorComponent interface

	// Returns the first equipped instance of a given type, or nullptr if none are found
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UMGEquipmentInstance* GetFirstInstanceOfType(TSubclassOf<UMGEquipmentInstance> InstanceType);

 	// Returns all equipped instances of a given type, or an empty array if none are found
 	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<UMGEquipmentInstance*> GetEquipmentInstancesOfType(TSubclassOf<UMGEquipmentInstance> InstanceType) const;

	template <typename T>
	T* GetFirstInstanceOfType()
	{
		return (T*)GetFirstInstanceOfType(T::StaticClass());
	}

protected:

	UFUNCTION()
	void OnRep_EquippedItem();

private:

	void UnequipCurrentItem();

public:
	// Delegate fired when a new item is equipped
	UPROPERTY(BlueprintAssignable)
	FMGEquipment_EquippedChanged OnEquipped;

	// Delegate fired when equipped item is unequipped
	UPROPERTY(BlueprintAssignable)
	FMGEquipment_EquippedChanged OnUnequipped;

private:

	UPROPERTY(Replicated)
	FMGEquipmentList EquipmentList;

	UPROPERTY(ReplicatedUsing=OnRep_EquippedItem)
	UMGEquipmentInstance* EquippedItem;
};
