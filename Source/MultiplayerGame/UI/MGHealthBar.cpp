// Copyright Artem Volovyi. All Rights Reserved.


#include "MGHealthBar.h"

void UMGHealthBar::NativeConstruct()
{
	Super::NativeOnInitialized();

	if (APlayerController* PlayerController = GetOwningPlayer())
	{
		PlayerController->OnPossessedPawnChanged.AddDynamic(this, &ThisClass::UMGHealthBar::OnPossessedPawnChanged);

		if (APawn* PlayerPawn = PlayerController->GetPawn())
		{
			OnPossessedPawnChanged(nullptr, PlayerPawn);
		}
	}
}

void UMGHealthBar::OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn)
{
	// Unbind health events from old pawn's health component
	if (const AMGCharacter* PlayerCharacter = Cast<AMGCharacter>(OldPawn))
	{
		if (UMGHealthComponent* HealthComponent = PlayerCharacter->FindComponentByClass<UMGHealthComponent>())
		{
			HealthComponent->OnHealthChanged.RemoveDynamic(this, &ThisClass::HandleHealthChanged);
			HealthComponent->OnMaxHealthChanged.RemoveDynamic(this, &ThisClass::HandleMaxHealthChanged);
		}
	}

	// Bind health event to new pawn's health component, save health values and initialize HealthBar visuals
	if (const AMGCharacter* PlayerCharacter = Cast<AMGCharacter>(NewPawn))
	{
		if (UMGHealthComponent* HealthComponent = PlayerCharacter->FindComponentByClass<UMGHealthComponent>())
		{
			HealthComponent->OnHealthChanged.AddDynamic(this, &ThisClass::HandleHealthChanged);
			HealthComponent->OnMaxHealthChanged.AddDynamic(this, &ThisClass::HandleMaxHealthChanged);

			Health = HealthComponent->GetHealth();
			HealthNormalized = HealthComponent->GetHealthNormalized();
			MaxHealth = HealthComponent->GetMaxHealth();

			InitializeHealthBarVisuals();
		}
	}
}

void UMGHealthBar::HandleHealthChanged(UMGHealthComponent* HealthComponent, float OldValue, float NewValue, AActor* Instigator)
{
	if (HealthBar)
	{
		HealthNormalized = HealthComponent->GetHealthNormalized();

		HealthBar->SetPercent(HealthNormalized);

		// Linearly interpolate between colors
		HealthBar->SetFillColorAndOpacity(FLinearColor::LerpUsingHSV(LowHealthColor, FullHealthColor, HealthNormalized));
	}

	if (HealthNumber)
	{
		HealthNumber->SetText(FText::AsNumber(NewValue));
	}
}

void UMGHealthBar::HandleMaxHealthChanged(UMGHealthComponent* HealthComponent, float OldValue, float NewValue, AActor* Instigator)
{
	if (SizeBox)
	{
		const float OldWidthOverrideScale = SizeBox->GetWidthOverride() / OldValue;

		const float NewWidthOverride = NewValue * OldWidthOverrideScale;

		SizeBox->SetWidthOverride(NewWidthOverride);
	}

	if (MaxHealthNumber)
	{
		MaxHealthNumber->SetText(FText::AsNumber(NewValue));
	}
}

void UMGHealthBar::InitializeHealthBarVisuals()
{
	if (HealthBar)
	{
		HealthBar->SetPercent(HealthNormalized);

		// Linearly interpolate between colors
		HealthBar->SetFillColorAndOpacity(FLinearColor::LerpUsingHSV(LowHealthColor, FullHealthColor, HealthNormalized));
	}

	if (HealthNumber)
	{
		HealthNumber->SetText(FText::AsNumber(Health));
	}

	if (MaxHealthNumber)
	{
		MaxHealthNumber->SetText(FText::AsNumber(MaxHealth));
	}
}
