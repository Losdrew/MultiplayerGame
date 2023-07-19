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
			AddWidgetToSlot(WidgetInSlot.Widget, WidgetInSlot.SlotTag);
		}
	}
}

void AMGHUD::AddWidgetToSlot(TSubclassOf<UUserWidget> WidgetClass, FGameplayTag SlotTag)
{
	if (LayoutUI)
	{
		UUserWidget* Widget = CreateWidget(PlayerOwner, WidgetClass);
		LayoutUI->AddWidgetToSlot(Widget, SlotTag);
	}
}

void AMGHUD::RemoveWidgetFromSlot(FGameplayTag SlotTag)
{
	if (LayoutUI)
	{
		LayoutUI->RemoveWidgetFromSlot(SlotTag);
	}
}
