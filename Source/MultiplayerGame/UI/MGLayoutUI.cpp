// Copyright Artem Volovyi. All Rights Reserved.

#include "MGLayoutUI.h"

#include "Blueprint/WidgetTree.h"
#include "GameplayTags.h"
#include "MGNamedSlotWithTag.h"

void UMGLayoutUI::SetWidgetToSlot(UUserWidget* Widget, FGameplayTag SlotTag)
{
	if (const UNamedSlot* NamedSlot = Cast<UNamedSlot>(Slots.FindRef(SlotTag)))
	{
		SetContentForSlot(NamedSlot->GetFName(), Widget);
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
