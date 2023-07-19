// Copyright Artem Volovyi. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTags.h"
#include "MGNamedSlotWithTag.h"
#include "MGLayoutUI.generated.h"

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

	virtual void AddWidgetToSlot(UUserWidget* Widget, FGameplayTag SlotTag);
	virtual void RemoveWidgetFromSlot(FGameplayTag SlotTag);

protected:

	virtual void NativeOnInitialized() override;

private:

	UPROPERTY()
	TMap<FGameplayTag, UMGNamedSlotWithTag*> Slots;
};
