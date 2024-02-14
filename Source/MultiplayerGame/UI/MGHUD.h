// Copyright Artem Volovyi. All Rights Reserved.

#pragma once

#include "GameFramework/HUD.h"

#include "MGHUD.generated.h"

class UMGLayoutUI;
struct FGameplayTag;

/**
 * AMGHUD
 *
 *	The base HUD class used by this project
 */
UCLASS()
class AMGHUD : public AHUD
{
	GENERATED_BODY()

protected:

	//~AActor interface
	virtual void BeginPlay() override;
	//~End of AActor interface

public:

	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	virtual UUserWidget* CreateWidgetInSlot(TSubclassOf<UUserWidget> WidgetClass, FGameplayTag SlotTag);

	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	virtual void ShowWidgetInSlot(FGameplayTag SlotTag);

	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	virtual void HideWidgetInSlot(FGameplayTag SlotTag);

	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	virtual void ClearSlot(FGameplayTag SlotTag);

public:

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UMGLayoutUI> LayoutUI;
};
