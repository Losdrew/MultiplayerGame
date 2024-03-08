// Copyright Artem Volovyi. All Rights Reserved.


#include "MGHealthBar.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "MGHealthComponent.h"

void UMGHealthBar::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	APlayerController* PlayerController = GetOwningPlayer();
	PlayerController->OnPossessedPawnChanged.AddUniqueDynamic(this, &ThisClass::OnPossessedPawnChanged);

	if (APawn* PlayerPawn = PlayerController->GetPawn())
	{
		OnPossessedPawnChanged(nullptr, PlayerPawn);
	}
}

void UMGHealthBar::OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn)
{
	// Unbind health events from old pawn's health component
	if (OldPawn != nullptr)
	{
		if (UMGHealthComponent* HealthComponent = OldPawn->FindComponentByClass<UMGHealthComponent>())
		{
			HealthComponent->OnHealthChanged.RemoveDynamic(this, &ThisClass::OnHealthChanged);
			HealthComponent->OnMaxHealthChanged.RemoveDynamic(this, &ThisClass::OnMaxHealthChanged);
		}
	}

	// Bind health events to new pawn's health component
	if (NewPawn != nullptr)
	{
		if (UMGHealthComponent* HealthComponent = NewPawn->FindComponentByClass<UMGHealthComponent>())
		{
			HealthComponent->OnHealthChanged.AddUniqueDynamic(this, &ThisClass::OnHealthChanged);
			HealthComponent->OnMaxHealthChanged.AddUniqueDynamic(this, &ThisClass::OnMaxHealthChanged);

			UpdateHealthBar(HealthComponent);
		}
	}
}

void UMGHealthBar::UpdateHealthBar(UMGHealthComponent* HealthComponent)
{
	OnHealthChanged(HealthComponent, 0.0f, HealthComponent->GetHealth(), nullptr);
	OnMaxHealthChanged(HealthComponent, 0.0f, HealthComponent->GetMaxHealth(), nullptr);
}

void UMGHealthBar::OnHealthChanged(UMGHealthComponent* HealthComponent, float OldValue, float NewValue, AActor* Instigator)
{
	float HealthNormalized = HealthComponent->GetHealthNormalized();
	HealthBar->SetPercent(HealthNormalized);
	HealthBar->SetFillColorAndOpacity(FLinearColor::LerpUsingHSV(LowHealthColor, FullHealthColor, HealthNormalized));
	HealthNumber->SetText(FText::AsNumber(NewValue));
}

void UMGHealthBar::OnMaxHealthChanged(UMGHealthComponent* HealthComponent, float OldValue, float NewValue, AActor* Instigator)
{
	MaxHealthNumber->SetText(FText::AsNumber(NewValue));
	OnHealthChanged(HealthComponent, 0.0f, HealthComponent->GetHealth(), nullptr);
}
