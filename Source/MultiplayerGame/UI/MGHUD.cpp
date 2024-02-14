// Copyright Artem Volovyi. All Rights Reserved.


#include "MGHUD.h"

#include "MGGameState.h"
#include "MGHUDDataAsset.h"
#include "MGLayoutUI.h"
#include "GameplayTagContainer.h"

void AMGHUD::BeginPlay()
{
	Super::BeginPlay();

	if (const AMGGameState* GameState = Cast<AMGGameState>(GetWorld()->GetGameState()))
	{
		LayoutUI = Cast<UMGLayoutUI>(CreateWidget(PlayerOwner, GameState->HUDDataAsset->Layout, "LayoutUI"));
		LayoutUI->AddToViewport();

		for (const FMGWidgetInSlot WidgetInSlot : GameState->HUDDataAsset->Widgets)
		{
			CreateWidgetInSlot(WidgetInSlot.Widget, WidgetInSlot.SlotTag);
		}
	}
}

UUserWidget* AMGHUD::CreateWidgetInSlot(TSubclassOf<UUserWidget> WidgetClass, FGameplayTag SlotTag)
{
	if (LayoutUI)
	{
		UUserWidget* Widget = CreateWidget(PlayerOwner, WidgetClass);
		LayoutUI->SetWidgetToSlot(Widget, SlotTag);
		return Widget;
	}

	return nullptr;
}

void AMGHUD::ShowWidgetInSlot(FGameplayTag SlotTag)
{
	if (LayoutUI)
	{
		LayoutUI->SetSlotVisibility(SlotTag, ESlateVisibility::Visible);
	}
}

void AMGHUD::HideWidgetInSlot(FGameplayTag SlotTag)
{
	if (LayoutUI)
	{
		LayoutUI->SetSlotVisibility(SlotTag, ESlateVisibility::Hidden);
	}
}

void AMGHUD::ClearSlot(FGameplayTag SlotTag)
{
	if (LayoutUI)
	{
		LayoutUI->SetWidgetToSlot(nullptr, SlotTag);
	}
}
