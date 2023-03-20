// Copyright Artem Volovyi. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/HorizontalBox.h"
#include "Components/SizeBox.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "MGCharacter.h"
#include "MGHealthBar.generated.h"

/**
 * 
 */
UCLASS()
class UMGHealthBar : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION()
	void OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn);

	UFUNCTION()
	void HandleHealthChanged(UMGHealthComponent* HealthComponent, float OldValue, float NewValue, AActor* Instigator);

	UFUNCTION()
	void HandleMaxHealthChanged(UMGHealthComponent* HealthComponent, float OldValue, float NewValue, AActor* Instigator);

	UFUNCTION()
	void InitializeHealthBarVisuals();

protected:

	virtual void NativeConstruct() override;

public:

	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* HorizontalBox;

	UPROPERTY(meta = (BindWidget))
	USizeBox* SizeBox;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthBar;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* HealthNumber;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* HealthDivider;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* MaxHealthNumber;

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
