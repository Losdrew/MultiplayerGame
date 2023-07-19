// Copyright Artem Volovyi. All Rights Reserved.

#include "UI/MGLayoutUI.h"
#include "UMG/Public/Blueprint/WidgetTree.h"

void UMGLayoutUI::AddWidgetToSlot(UUserWidget* Widget, FGameplayTag SlotTag)
{
	if (const UNamedSlot* NamedSlot = Cast<UNamedSlot>(Slots.FindRef(SlotTag)))
	{
		SetContentForSlot(NamedSlot->GetFName(), Widget);
	}
}

void UMGLayoutUI::RemoveWidgetFromSlot(FGameplayTag SlotTag)
{
	if (const UNamedSlot* NamedSlot = Cast<UNamedSlot>(Slots.FindRef(SlotTag)))
	{
		SetContentForSlot(NamedSlot->GetFName(), nullptr);
	}
}

void UMGLayoutUI::NativeOnInitialized()
{
	TArray<FName> SlotNames;

	GetSlotNames(SlotNames);

	if (WidgetTree)
	{
		for (FName SlotName : SlotNames)
		{
			if (UMGNamedSlotWithTag* NamedSlot = Cast<UMGNamedSlotWithTag>(WidgetTree->FindWidget(SlotName)))
			{
				Slots.Add(NamedSlot->SlotTag, NamedSlot);
			}
		}
	}
}
