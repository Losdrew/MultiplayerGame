// Copyright Artem Volovyi. All Rights Reserved.


#include "UI/MGHUD.h"
#include "MGGameState.h"

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

void AMGHUD::ClearSlot(FGameplayTag SlotTag)
{
	if (LayoutUI)
	{
		LayoutUI->SetWidgetToSlot(nullptr, SlotTag);
	}
}
