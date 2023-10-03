// Copyright Artem Volovyi. All Rights Reserved.


#include "MGHealthBar.h"

#include "Components/SizeBox.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "MGCharacter.h"
#include "MGHealthComponent.h"

void UMGHealthBar::NativeConstruct()
{
	Super::NativeConstruct();

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
			HealthComponent->OnHealthChanged.RemoveDynamic(this, &ThisClass::OnHealthChanged);
			HealthComponent->OnMaxHealthChanged.RemoveDynamic(this, &ThisClass::OnMaxHealthChanged);
		}
	}

	// Bind health events to new pawn's health component, save health values and initialize HealthBar visuals
	if (const AMGCharacter* PlayerCharacter = Cast<AMGCharacter>(NewPawn))
	{
		if (UMGHealthComponent* HealthComponent = PlayerCharacter->FindComponentByClass<UMGHealthComponent>())
		{
			HealthComponent->OnHealthChanged.AddUniqueDynamic(this, &ThisClass::OnHealthChanged);
			HealthComponent->OnMaxHealthChanged.AddUniqueDynamic(this, &ThisClass::OnMaxHealthChanged);

			Health = HealthComponent->GetHealth();
			HealthNormalized = HealthComponent->GetHealthNormalized();
			MaxHealth = HealthComponent->GetMaxHealth();

			InitializeHealthBarVisuals();
		}
	}
}

void UMGHealthBar::OnHealthChanged(UMGHealthComponent* HealthComponent, float OldValue, float NewValue, AActor* Instigator)
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

void UMGHealthBar::OnMaxHealthChanged(UMGHealthComponent* HealthComponent, float OldValue, float NewValue, AActor* Instigator)
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
