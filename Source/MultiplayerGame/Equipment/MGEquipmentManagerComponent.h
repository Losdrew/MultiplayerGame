// Copyright Artem Volovyi. All Rights Reserved.

#pragma once

#include "Net/Serialization/FastArraySerializer.h"
#include "MGAbilitySet.h"
#include "MGEquipmentDefinition.h"

#include "MGEquipmentManagerComponent.generated.h"

class UMGEquipmentInstance;
class UMGAbilitySystemComponent;
class UMGEquipmentManagerComponent;
struct FMGEquipmentList;

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

	bool operator==(const FMGAppliedEquipmentEntry& Other) const
	{
		return EquipmentDefinition == Other.EquipmentDefinition
			&& Instance == Other.Instance
			&& GrantedHandles.StaticStruct() == Other.GrantedHandles.StaticStruct();
	}

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
	void ActivateEntry(const UMGEquipmentInstance* EquipmentInstance);
	void DeactivateEntry(const UMGEquipmentInstance* EquipmentInstance);
	void RemoveEntry(const UMGEquipmentInstance* EquipmentInstance);

private:

	friend UMGEquipmentManagerComponent;

	UMGAbilitySystemComponent* GetAbilitySystemComponent() const;

	FMGAppliedEquipmentEntry* FindEntryByInstance(const UMGEquipmentInstance* EquipmentInstance);

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
	UMGEquipmentInstance* AddItem(TSubclassOf<UMGEquipmentDefinition> EquipmentDefinition);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void RemoveItem(UMGEquipmentInstance* ItemInstance);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void EquipItem(const UMGEquipmentInstance* ItemInstance);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void UnequipCurrentItem();

	//~UActorComponent interface
	virtual void InitializeComponent() override;
	virtual void UninitializeComponent() override;
	virtual void ReadyForReplication() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	//~End of UActorComponent interface

	UFUNCTION(BlueprintCallable)
	UMGEquipmentInstance* GetEquippedItem() const { return EquippedItem; }

	// Returns the first equipped instance of a given type, or nullptr if none are found
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UMGEquipmentInstance* GetFirstInstanceOfType(TSubclassOf<UMGEquipmentInstance> InstanceType) const;

	// Returns the first equipped instance by equipment definition, or nullptr if none are found
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UMGEquipmentInstance* GetFirstInstanceByDefinition(TSubclassOf<UMGEquipmentDefinition> EquipmentDefinition) const;

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
	void OnRep_EquippedItem(UMGEquipmentInstance* OldItem);

public:
	// Delegate fired when a new item is equipped
	UPROPERTY(BlueprintAssignable)
	FMGEquipment_EquippedChanged OnEquipped;

	// Delegate fired when equipped item is unequipped
	UPROPERTY(BlueprintAssignable)
	FMGEquipment_EquippedChanged OnUnequipped;

public:

	UPROPERTY(ReplicatedUsing = OnRep_EquippedItem)
	TObjectPtr<UMGEquipmentInstance> EquippedItem;

protected:

	UPROPERTY(Replicated)
	FMGEquipmentList EquipmentList;
	
};
