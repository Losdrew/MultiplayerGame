// Copyright Artem Volovyi. All Rights Reserved.


#include "MGHUD.h"

#include "GameplayTagContainer.h"
#include "MGGameState.h"
#include "MGHUDDataAsset.h"
#include "MGLayoutUI.h"

void AMGHUD::BeginPlay()
{
	Super::BeginPlay();

	if (const AMGGameState* GameState = Cast<AMGGameState>(GetWorld()->GetGameState()))
	{
		CreateLayout(GameState->HUDDataAsset->Layout);

		for (const FMGWidgetInSlot WidgetInSlot : GameState->HUDDataAsset->Widgets)
		{
			CreateWidgetInSlot(WidgetInSlot.Widget, WidgetInSlot.SlotTag);
		}
	}
}

UUserWidget* AMGHUD::CreateLayout(TSubclassOf<UUserWidget> LayoutWidgetClass)
{
	if (LayoutWidgetClass == nullptr)
	{
		return nullptr;
	}

	LayoutUI = Cast<UMGLayoutUI>(CreateWidget(PlayerOwner, LayoutWidgetClass, "LayoutUI"));
	LayoutUI->AddToViewport();

	return LayoutUI;
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
