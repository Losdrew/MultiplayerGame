// Copyright Artem Volovyi. All Rights Reserved.


#include "MGQuickBarComponent.h"

#include "MGEquipmentDefinition.h"
#include "MGEquipmentInstance.h"
#include "MGEquipmentManagerComponent.h"
#include "Net/UnrealNetwork.h"

UMGQuickBarComponent::UMGQuickBarComponent()
{
	SetIsReplicatedByDefault(true);
}

void UMGQuickBarComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, Slots);
	DOREPLIFETIME(ThisClass, ActiveSlotIndex);
}

void UMGQuickBarComponent::BeginPlay()
{
	if (Slots.Num() < QuickSlotEquipmentDefinitions.Num())
	{
		Slots.AddDefaulted(QuickSlotEquipmentDefinitions.Num() - Slots.Num());
	}

	Super::BeginPlay();
}

void UMGQuickBarComponent::CycleActiveSlotForward()
{
	// No point in cycling if we have only one slot or none
	if (Slots.Num() < 2)
	{
		return;
	}

	const int32 OldIndex = (ActiveSlotIndex < 0 ? Slots.Num() - 1 : ActiveSlotIndex);
	int32 NewIndex = ActiveSlotIndex;

	// Cycle forward through slots until we find a non-empty one
	do
	{
		NewIndex = (NewIndex + 1) % Slots.Num();
		if (Slots[NewIndex] != nullptr)
		{
			SetActiveSlotIndex(NewIndex);
			return;
		}
	} while (NewIndex != OldIndex);
}

void UMGQuickBarComponent::CycleActiveSlotBackward()
{
	// No point in cycling if we have only one slot or none
	if (Slots.Num() < 2)
	{
		return;
	}

	const int32 OldIndex = (ActiveSlotIndex < 0 ? Slots.Num() - 1 : ActiveSlotIndex);
	int32 NewIndex = ActiveSlotIndex;

	// Cycle backward through slots until we find a non-empty one
	do
	{
		NewIndex = (NewIndex - 1 + Slots.Num()) % Slots.Num();
		if (Slots[NewIndex] != nullptr)
		{
			SetActiveSlotIndex(NewIndex);
			return;
		}
	} while (NewIndex != OldIndex);
}

void UMGQuickBarComponent::SetActiveSlotIndex_Implementation(int32 NewIndex)
{
	if (Slots[NewIndex] != nullptr)
	{
		if (Slots.IsValidIndex(NewIndex) && (ActiveSlotIndex != NewIndex))
		{
			UnequipItemInActiveSlot();
			ActiveSlotIndex = NewIndex;
			EquipItemInActiveSlot();
			OnRep_ActiveSlotIndex();
		}
	}
}

UMGEquipmentInstance* UMGQuickBarComponent::AddItemToSlot(TSubclassOf<UMGEquipmentDefinition> EquipmentClass)
{
	int32 SlotIndex = FindQuickSlotByEquipmentDefinition(EquipmentClass);

	if (Slots.IsValidIndex(SlotIndex) && Slots[SlotIndex] == nullptr)
	{
		if (UMGEquipmentManagerComponent* EquipmentManager = GetEquipmentManager())
		{
			UMGEquipmentInstance* EquipmentInstance = EquipmentManager->AddItem(EquipmentClass);
			if (EquipmentInstance != nullptr)
			{
				Slots[SlotIndex] = EquipmentInstance;
				SetActiveSlotIndex(SlotIndex);
				OnRep_Slots();
			}
			return EquipmentInstance;
		}
	}

	return nullptr;
}

void UMGQuickBarComponent::RemoveItemFromSlot(int32 SlotIndex)
{
	if (ActiveSlotIndex == SlotIndex)
	{
		UnequipItemInActiveSlot();
		ActiveSlotIndex = -1;
	}

	if (Slots.IsValidIndex(SlotIndex) && Slots[SlotIndex] != nullptr)
	{
		if (UMGEquipmentManagerComponent* EquipmentManager = GetEquipmentManager())
		{
			EquipmentManager->RemoveItem(Slots[SlotIndex]);
			Slots[SlotIndex] = nullptr;
			OnRep_Slots();
		}
	}
}

int32 UMGQuickBarComponent::FindQuickSlotByEquipmentDefinition(TSubclassOf<UMGEquipmentDefinition> EquipmentDefinition)
{
	for (const FMGQuickSlotEquipmentDefinition& Current : QuickSlotEquipmentDefinitions)
	{
		if (Current.EquipmentDefinition == EquipmentDefinition)
		{
			return Current.QuickSlot.GetIntValue();
		}
	}

	return INDEX_NONE;
}

UMGEquipmentManagerComponent* UMGQuickBarComponent::GetEquipmentManager() const
{
	if (AController* Controller = Cast<AController>(GetOwner()))
	{
		if (APawn* Pawn = Cast<APawn>(Controller->GetPawn()))
		{
			return Pawn->FindComponentByClass<UMGEquipmentManagerComponent>();
		}
	}
	return nullptr;
}

void UMGQuickBarComponent::EquipItemInActiveSlot()
{
	check(Slots.IsValidIndex(ActiveSlotIndex));
	check(GetEquipmentManager()->EquippedItem == nullptr);

	if (UMGEquipmentInstance* ActiveSlotEquipment = Slots[ActiveSlotIndex])
	{
		if (UMGEquipmentManagerComponent* EquipmentManager = GetEquipmentManager())
		{
			EquipmentManager->EquipItem(ActiveSlotEquipment);
		}
	}
}

void UMGQuickBarComponent::UnequipItemInActiveSlot()
{
	if (UMGEquipmentManagerComponent* EquipmentManager = GetEquipmentManager())
	{
		EquipmentManager->UnequipCurrentItem();
	}
}

void UMGQuickBarComponent::OnRep_Slots()
{
	OnSlotsChanged.Broadcast(Slots);
}

void UMGQuickBarComponent::OnRep_ActiveSlotIndex()
{
	OnActiveSlotIndexChanged.Broadcast(ActiveSlotIndex);
}