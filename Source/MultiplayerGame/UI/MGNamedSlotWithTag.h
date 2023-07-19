// Copyright Artem Volovyi. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTags.h"
#include "Components/NamedSlot.h"
#include "MGNamedSlotWithTag.generated.h"

/**
 * UMGNamedSlotWithTag
 *
 *	Named slot widget with a gameplay tag
 */
UCLASS()
class UMGNamedSlotWithTag : public UNamedSlot
{
	GENERATED_BODY()

	virtual TSharedRef<SWidget> RebuildWidget() override;

public:

	UPROPERTY(EditAnywhere, Category = "Tags")
	FGameplayTag SlotTag;
};
