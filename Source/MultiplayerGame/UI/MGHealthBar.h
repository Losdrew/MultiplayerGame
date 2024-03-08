// Copyright Artem Volovyi. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"

#include "MGHealthBar.generated.h"

class UTextBlock;
class UProgressBar;
class UMGHealthComponent;

/**
 * UMGHealthBar
 *
 *	Widget that displays information about a pawn's health
 */
UCLASS()
class UMGHealthBar : public UUserWidget
{
	GENERATED_BODY()

protected:

	//~UUserWidget interface
	virtual void NativeOnInitialized() override;
	//~End of UUserWidget interface

	void UpdateHealthBar(UMGHealthComponent* HealthComponent);

	UFUNCTION()
	void OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn);
	
	UFUNCTION()
	void OnHealthChanged(UMGHealthComponent* HealthComponent, float OldValue, float NewValue, AActor* Instigator);

	UFUNCTION()
	void OnMaxHealthChanged(UMGHealthComponent* HealthComponent, float OldValue, float NewValue, AActor* Instigator);

public:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> HealthBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> HealthNumber;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> MaxHealthNumber;

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Color")
	FLinearColor FullHealthColor;

	UPROPERTY(EditDefaultsOnly, Category = "Color")
	FLinearColor LowHealthColor;
};
