// Copyright Artem Volovyi. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"

#include "MGLayoutUI.generated.h"

class UMGNamedSlotWithTag;

/**
 * UMGLayoutUI
 *
 *	Layout widget on which slots with gameplay tags should be placed
 */
UCLASS()
class UMGLayoutUI : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void SetWidgetToSlot(UUserWidget* Widget, FGameplayTag SlotTag);
	virtual void SetSlotVisibility(FGameplayTag SlotTag, ESlateVisibility NewVisibility);

protected:

	virtual void NativeOnInitialized() override;

private:

	UPROPERTY()
	TMap<FGameplayTag, TObjectPtr<UMGNamedSlotWithTag>> Slots;
};
