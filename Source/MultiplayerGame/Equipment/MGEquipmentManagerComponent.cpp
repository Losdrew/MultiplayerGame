// Copyright Artem Volovyi. All Rights Reserved.


#include "MGEquipmentManagerComponent.h"

#include "MGAbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "MGAbilitySet.h"
#include "Net/UnrealNetwork.h"
#include "MGEquipmentInstance.h"
#include "MGEquipmentDefinition.h"

//////////////////////////////////////////////////////////////////////
// FMGEquipmentList

void FMGEquipmentList::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
 	for (const int32 Index : RemovedIndices)
 	{
 		const FMGAppliedEquipmentEntry& Entry = Items[Index];
		if (Entry.Instance != nullptr)
		{
			Entry.Instance->OnUnequipped();
		}
 	}
}

void FMGEquipmentList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for (const int32 Index : AddedIndices)
	{
		const FMGAppliedEquipmentEntry& Entry = Items[Index];
		if (Entry.Instance != nullptr)
		{
			Entry.Instance->OnEquipped();
		}
	}
}

void FMGEquipmentList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
}

UMGAbilitySystemComponent* FMGEquipmentList::GetAbilitySystemComponent() const
{
	check(OwnerComponent);
	const AActor* OwningActor = OwnerComponent->GetOwner();
	return Cast<UMGAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwningActor));
}

UMGEquipmentInstance* FMGEquipmentList::AddEntry(TSubclassOf<UMGEquipmentDefinition> EquipmentDefinition)
{
	UMGEquipmentInstance* Result = nullptr;

	check(EquipmentDefinition != nullptr);
 	check(OwnerComponent);
	check(OwnerComponent->GetOwner()->HasAuthority());
	
	const UMGEquipmentDefinition* EquipmentDef = GetDefault<UMGEquipmentDefinition>(EquipmentDefinition);

	TSubclassOf<UMGEquipmentInstance> InstanceType = EquipmentDef->InstanceType;
	if (InstanceType == nullptr)
	{
		InstanceType = UMGEquipmentInstance::StaticClass();
	}
	
	FMGAppliedEquipmentEntry& NewEntry = Items.AddDefaulted_GetRef();
	NewEntry.EquipmentDefinition = EquipmentDefinition;
	NewEntry.Instance = NewObject<UMGEquipmentInstance>(OwnerComponent->GetOwner(), InstanceType);
	Result = NewEntry.Instance;

	if (UMGAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		for (const TObjectPtr<const UMGAbilitySet> AbilitySet : EquipmentDef->AbilitySetsToGrant)
		{
			AbilitySet->GiveToAbilitySystem(ASC, &NewEntry.GrantedHandles, Result);
		}
	}

	Result->SpawnEquipmentActors(EquipmentDef->ActorsToSpawn);

	MarkItemDirty(NewEntry);

	return Result;
}

void FMGEquipmentList::RemoveEntry(UMGEquipmentInstance* Instance)
{
	for (auto EntryIt = Items.CreateIterator(); EntryIt; ++EntryIt)
	{
		FMGAppliedEquipmentEntry& Entry = *EntryIt;
		if (Entry.Instance == Instance)
		{
			if (UMGAbilitySystemComponent* ASC = GetAbilitySystemComponent())
			{
				Entry.GrantedHandles.TakeFromAbilitySystem(ASC);
			}

			Instance->DestroyEquipmentActors();

			EntryIt.RemoveCurrent();
			MarkArrayDirty();
		}
	}
}

//////////////////////////////////////////////////////////////////////
// UMGEquipmentManagerComponent

UMGEquipmentManagerComponent::UMGEquipmentManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, EquipmentList(this)
{
	SetIsReplicatedByDefault(true);
	bWantsInitializeComponent = true;
	bReplicateUsingRegisteredSubObjectList = true;
}

void UMGEquipmentManagerComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, EquipmentList);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, EquippedItem, COND_OwnerOnly, REPNOTIFY_OnChanged);
}

UMGEquipmentInstance* UMGEquipmentManagerComponent::EquipItem(TSubclassOf<UMGEquipmentDefinition> EquipmentClass)
{
	ensure(EquipmentClass);

	UMGEquipmentInstance* Result = nullptr;

	Result = EquipmentList.AddEntry(EquipmentClass);

	if (Result != nullptr)
	{
		Result->OnEquipped();

		EquippedItem = Result;

		if (IsReadyForReplication())
		{
			AddReplicatedSubObject(Result);
			AddReplicatedSubObject(EquippedItem);
			OnRep_EquippedItem();
		}
	}

	return Result;
}

void UMGEquipmentManagerComponent::UnequipItem(UMGEquipmentInstance* ItemInstance)
{
	if (ItemInstance)
	{
		RemoveReplicatedSubObject(ItemInstance);
		ItemInstance->OnUnequipped();
		EquipmentList.RemoveEntry(ItemInstance);
	}
}

void UMGEquipmentManagerComponent::UnequipCurrentItem()
{
	if (EquippedItem)
	{
		RemoveReplicatedSubObject(EquippedItem);
		EquippedItem = nullptr;
		OnRep_EquippedItem();
	}
}

void UMGEquipmentManagerComponent::InitializeComponent()
{
	Super::InitializeComponent();
}

void UMGEquipmentManagerComponent::UninitializeComponent()
{
	TArray<UMGEquipmentInstance*> AllEquipmentInstances;

	// Gathering all instances before removal to avoid side effects affecting the equipment list iterator	
	for (const FMGAppliedEquipmentEntry& Entry : EquipmentList.Items)
	{
		AllEquipmentInstances.Add(Entry.Instance);
	}

	for (UMGEquipmentInstance* EquipInstance : AllEquipmentInstances)
	{
		UnequipItem(EquipInstance);
	}

	UnequipCurrentItem();

	Super::UninitializeComponent();
}

void UMGEquipmentManagerComponent::ReadyForReplication()
{
	Super::ReadyForReplication();

	// Register existing UMGEquipmentInstances
	for (const FMGAppliedEquipmentEntry& Entry : EquipmentList.Items)
	{
		UMGEquipmentInstance* Instance = Entry.Instance;

		if (IsValid(Instance))
		{
			AddReplicatedSubObject(Instance);
		}
	}
}

UMGEquipmentInstance* UMGEquipmentManagerComponent::GetFirstInstanceOfType(TSubclassOf<UMGEquipmentInstance> InstanceType)
{
	for (FMGAppliedEquipmentEntry& Entry : EquipmentList.Items)
	{
		if (UMGEquipmentInstance* Instance = Entry.Instance)
		{
			if (Instance->IsA(InstanceType))
			{
				return Instance;
			}
		}
	}

	return nullptr;
}

TArray<UMGEquipmentInstance*> UMGEquipmentManagerComponent::GetEquipmentInstancesOfType(TSubclassOf<UMGEquipmentInstance> InstanceType) const
{
	TArray<UMGEquipmentInstance*> Results;
	for (const FMGAppliedEquipmentEntry& Entry : EquipmentList.Items)
	{
		if (UMGEquipmentInstance* Instance = Entry.Instance)
		{
			if (Instance->IsA(InstanceType))
			{
				Results.Add(Instance);
			}
		}
	}
	return Results;
}

void UMGEquipmentManagerComponent::OnRep_EquippedItem()
{
	if (EquippedItem)
	{
		OnEquipped.Broadcast(this, EquippedItem);
	}
	else
	{
		OnUnequipped.Broadcast(this, nullptr);
	}
}
