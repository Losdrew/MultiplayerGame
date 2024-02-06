// Copyright Artem Volovyi. All Rights Reserved.


#include "MGEquipmentManagerComponent.h"

#include "AbilitySystemGlobals.h"
#include "MGAbilitySet.h"
#include "MGAbilitySystemComponent.h"
#include "MGEquipmentDefinition.h"
#include "MGEquipmentInstance.h"
#include "Net/UnrealNetwork.h"

//////////////////////////////////////////////////////////////////////
// FMGEquipmentList

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
	
	const UMGEquipmentDefinition* EquipmentCDO = GetDefault<UMGEquipmentDefinition>(EquipmentDefinition);

	TSubclassOf<UMGEquipmentInstance> InstanceType = EquipmentCDO->InstanceType;
	if (InstanceType == nullptr)
	{
		InstanceType = UMGEquipmentInstance::StaticClass();
	}
	
	FMGAppliedEquipmentEntry& NewEntry = Items.AddDefaulted_GetRef();
	NewEntry.EquipmentDefinition = EquipmentDefinition;
	NewEntry.Instance = NewObject<UMGEquipmentInstance>(OwnerComponent->GetOwner(), InstanceType);
	Result = NewEntry.Instance;

	NewEntry.Instance->SpawnEquipmentActors(EquipmentCDO->ActorsToSpawn);
	NewEntry.Instance->SetEquipmentActorsVisibility(false);

	MarkItemDirty(NewEntry);

	return Result;
}

void FMGEquipmentList::ActivateEntry(const UMGEquipmentInstance* EquipmentInstance)
{
	if (FMGAppliedEquipmentEntry* Entry = FindEntryByInstance(EquipmentInstance))
	{
		const UMGEquipmentDefinition* EquipmentCDO = GetDefault<UMGEquipmentDefinition>(Entry->EquipmentDefinition);

		if (UMGAbilitySystemComponent* ASC = GetAbilitySystemComponent())
		{
			for (const TObjectPtr<const UMGAbilitySet> AbilitySet : EquipmentCDO->AbilitySetsToGrant)
			{
				AbilitySet->GiveToAbilitySystem(ASC, &Entry->GrantedHandles, Entry->Instance);
			}
		}

		Entry->Instance->SetEquipmentActorsVisibility(true);
	}
}

void FMGEquipmentList::DeactivateEntry(const UMGEquipmentInstance* EquipmentInstance)
{
	if (FMGAppliedEquipmentEntry* Entry = FindEntryByInstance(EquipmentInstance))
	{
		if (UMGAbilitySystemComponent* ASC = GetAbilitySystemComponent())
		{
			Entry->GrantedHandles.TakeFromAbilitySystem(ASC);
		}

		Entry->Instance->SetEquipmentActorsVisibility(false);
	}
}

void FMGEquipmentList::RemoveEntry(const UMGEquipmentInstance* EquipmentInstance)
{
	if (FMGAppliedEquipmentEntry* Entry = FindEntryByInstance(EquipmentInstance))
	{
		if (UMGAbilitySystemComponent* ASC = GetAbilitySystemComponent())
		{
			Entry->GrantedHandles.TakeFromAbilitySystem(ASC);
		}

		Entry->Instance->DestroyEquipmentActors();
		
		Items.RemoveSingle(*Entry);
		MarkArrayDirty();
	}
}

FMGAppliedEquipmentEntry* FMGEquipmentList::FindEntryByInstance(const UMGEquipmentInstance* EquipmentInstance)
{
	for (FMGAppliedEquipmentEntry& Entry : Items)
	{
		if (Entry.Instance == EquipmentInstance)
		{
			return &Entry;
		}
	}
	return nullptr;
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
	PrimaryComponentTick.bStartWithTickEnabled = true;
	PrimaryComponentTick.bCanEverTick = true;
}

void UMGEquipmentManagerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, EquipmentList);
	DOREPLIFETIME(ThisClass, EquippedItem);
}

UMGEquipmentInstance* UMGEquipmentManagerComponent::AddItem(TSubclassOf<UMGEquipmentDefinition> EquipmentClass)
{
	ensure(EquipmentClass);

	UMGEquipmentInstance* Result = EquipmentList.AddEntry(EquipmentClass);

	if (Result != nullptr)
	{
		if (IsReadyForReplication())
		{
			AddReplicatedSubObject(Result);
		}
	}

	return Result;
}

void UMGEquipmentManagerComponent::RemoveItem(UMGEquipmentInstance* ItemInstance)
{
	if (ItemInstance == EquippedItem)
	{
		UnequipCurrentItem();
	}

	if (ItemInstance != nullptr)
	{
		RemoveReplicatedSubObject(ItemInstance);
		EquipmentList.RemoveEntry(ItemInstance);
	}
}

void UMGEquipmentManagerComponent::EquipItem(const UMGEquipmentInstance* ItemInstance)
{
	if (ItemInstance == nullptr)
	{
		return;
	}

	if (UMGEquipmentInstance* ExistingItem = GetFirstInstanceOfType(ItemInstance->GetClass()))
	{
		UMGEquipmentInstance* PreviousItem = EquippedItem;
		EquipmentList.ActivateEntry(ExistingItem);
		EquippedItem = ExistingItem;

		if (IsReadyForReplication())
		{
			AddReplicatedSubObject(EquippedItem);
			OnRep_EquippedItem(PreviousItem);
		}
	}
}

void UMGEquipmentManagerComponent::UnequipCurrentItem()
{
	if (EquippedItem == nullptr)
	{
		return;
	}

	UMGEquipmentInstance* PreviousItem = EquippedItem;
	RemoveReplicatedSubObject(EquippedItem);
	EquipmentList.DeactivateEntry(EquippedItem);
	EquippedItem = nullptr;
	OnRep_EquippedItem(PreviousItem);
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
		RemoveItem(EquipInstance);
	}

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

void UMGEquipmentManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (UMGEquipmentInstance* CurrentEquipment = Cast<UMGEquipmentInstance>(EquippedItem))
	{
		CurrentEquipment->Tick(DeltaTime);
	}
}

UMGEquipmentInstance* UMGEquipmentManagerComponent::GetFirstInstanceOfType(TSubclassOf<UMGEquipmentInstance> InstanceType) const
{
	for (const FMGAppliedEquipmentEntry& Entry : EquipmentList.Items)
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

UMGEquipmentInstance* UMGEquipmentManagerComponent::GetFirstInstanceByDefinition(TSubclassOf<UMGEquipmentDefinition> EquipmentDefinition) const
{
	for (const FMGAppliedEquipmentEntry& Entry : EquipmentList.Items)
	{
		if (Entry.EquipmentDefinition == EquipmentDefinition)
		{
			return Entry.Instance;
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

void UMGEquipmentManagerComponent::OnRep_EquippedItem(UMGEquipmentInstance* PreviousItem)
{
	if (PreviousItem != nullptr)
	{
		PreviousItem->OnUnequipped();
		OnUnequipped.Broadcast(this, PreviousItem);
	}

	if (EquippedItem != nullptr)
	{
		EquippedItem->OnEquipped();
		OnEquipped.Broadcast(this, EquippedItem);
	}
}
