// Copyright Artem Volovyi. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTags.h"
#include "MGLayoutUI.h"
#include "MGHUDDataAsset.generated.h"

/**
 * FMGWidgetInSlot
 *
 *	Widget with a slot it should be placed in
 */
USTRUCT(BlueprintType)
struct FMGWidgetInSlot
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> Widget = nullptr;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag SlotTag;
};

/**
 * UMGHUDDataAsset
 *
 *	Widgets to be displayed on the screen immediately for this gamemode 
 */
UCLASS(Blueprintable, Const)
class UMGHUDDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UMGLayoutUI> Layout;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TArray<FMGWidgetInSlot> Widgets;
};
