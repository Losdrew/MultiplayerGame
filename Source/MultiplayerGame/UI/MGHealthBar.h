// Copyright Artem Volovyi. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"

#include "MGHealthBar.generated.h"

class USizeBox;
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

public:

	UFUNCTION()
	void OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn);

	UFUNCTION()
	void InitializeHealthBarVisuals();

protected:

	UFUNCTION(BlueprintCallable)
	void OnHealthChanged(UMGHealthComponent* HealthComponent, float OldValue, float NewValue, AActor* Instigator);

	UFUNCTION(BlueprintCallable)
	void OnMaxHealthChanged(UMGHealthComponent* HealthComponent, float OldValue, float NewValue, AActor* Instigator);

	virtual void NativeConstruct() override;

public:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USizeBox> SizeBox;

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

	UPROPERTY(BlueprintReadOnly)
	float Health;

	UPROPERTY(BlueprintReadOnly)
	float HealthNormalized;

	UPROPERTY(BlueprintReadOnly)
	float MaxHealth;
};
