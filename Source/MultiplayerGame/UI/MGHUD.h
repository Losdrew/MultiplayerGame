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

	UFUNCTION(BlueprintCallable)
	UUserWidget* CreateLayout(TSubclassOf<UUserWidget> LayoutWidgetClass);

	UFUNCTION(BlueprintCallable)
	UUserWidget* CreateWidgetInSlot(TSubclassOf<UUserWidget> WidgetClass, FGameplayTag SlotTag);

	UFUNCTION(BlueprintCallable)
	void ShowWidgetInSlot(FGameplayTag SlotTag);

	UFUNCTION(BlueprintCallable)
	void HideWidgetInSlot(FGameplayTag SlotTag);

	UFUNCTION(BlueprintCallable)
	void ClearSlot(FGameplayTag SlotTag);

public:

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UMGLayoutUI> LayoutUI;
};
