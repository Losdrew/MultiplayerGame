// Copyright Artem Volovyi. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"

#include "MGKillMessage.generated.h"

class UImage;
class UTextBlock;
struct FGameplayEffectContextHandle;

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
	void OnPlayerKilled(AActor* KillerActor, AActor* AssistActor, AActor* KilledActor, const FGameplayEffectContextHandle& DamageContext);

	// Event for cosmetic actions related to the kill
	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic)
	void ReceiveOnPlayerKilled(AActor* KillerActor, AActor* AssistActor, AActor* KilledActor);

public:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> KillerName;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> KillerAssistSeparator;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> AssistName;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> KilledName;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> WeaponImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> KillTypeImage;

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Icons")
	TObjectPtr<UTexture2D> KillIcon;

	UPROPERTY(EditDefaultsOnly, Category = "Icons")
	TObjectPtr<UTexture2D> WeakSpotKillIcon;
};
