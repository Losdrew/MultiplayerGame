// Copyright Artem Volovyi. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MGLayoutUI.h"
#include "MGHUDDataAsset.h"
#include "MGHUD.generated.h"

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
	virtual void ClearSlot(FGameplayTag SlotTag);

public:

	UPROPERTY(BlueprintReadOnly)
	UMGLayoutUI* LayoutUI;
};
