// Copyright Artem Volovyi. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "MGKillMessage.generated.h"

/**
 * UMGKillMessage
 *
 *	A single entry to the kill feed.
 *	Displays information on who and how killed who.
 */
UCLASS()
class UMGKillMessage : public UUserWidget
{
	GENERATED_BODY()

public:
	// Updates the widget properties with the info about the kill
	UFUNCTION(BlueprintCallable)
	void OnPlayerKilled(AActor* KillerActor, AActor* KilledActor, const FGameplayEffectContextHandle& DamageContext);

	// Event for cosmetic actions related to the kill
	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic)
	void ReceiveOnPlayerKilled(AActor* KillerActor, AActor* KilledActor, const FGameplayEffectContextHandle& DamageContext);

public:

	UPROPERTY(meta = (BindWidget))
	UTextBlock* KillerName;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* KilledName;

	UPROPERTY(meta = (BindWidget))
	UImage* WeaponImage;

	UPROPERTY(meta = (BindWidget))
	UImage* KillTypeImage;

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Icons")
	UTexture2D* KillIcon;

	UPROPERTY(EditDefaultsOnly, Category = "Icons")
	UTexture2D* WeakSpotKillIcon;
};
