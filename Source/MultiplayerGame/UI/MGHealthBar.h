// Copyright Artem Volovyi. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
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
	void InitializeHealthBarVisuals();

protected:

	UFUNCTION(BlueprintCallable)
	void OnHealthChanged(UMGHealthComponent* HealthComponent, float OldValue, float NewValue, AActor* Instigator);

	UFUNCTION(BlueprintCallable)
	void OnMaxHealthChanged(UMGHealthComponent* HealthComponent, float OldValue, float NewValue, AActor* Instigator);

	virtual void NativeConstruct() override;

public:

	UPROPERTY(meta = (BindWidget))
	USizeBox* SizeBox;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthBar;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* HealthNumber;

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
