// Copyright Artem Volovyi. All Rights Reserved.


#include "MGArmorStatus.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "MGArmorSet.h"
#include "MGCharacter.h"

void UMGArmorStatus::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	APlayerController* PlayerController = GetOwningPlayer();
	PlayerController->OnPossessedPawnChanged.AddUniqueDynamic(this, &ThisClass::OnPossessedPawnChanged);

	if (APawn* PlayerPawn = PlayerController->GetPawn())
	{
		OnPossessedPawnChanged(nullptr, PlayerPawn);
	}
}

void UMGArmorStatus::OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn)
{
	// Unbind armor events from old pawn's armor set
	if (UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OldPawn))
	{
		if (const UMGArmorSet* ArmorSet = AbilitySystemComponent->GetSet<UMGArmorSet>())
		{
			AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(ArmorSet->GetArmorAttribute()).Remove(ArmorChangedHandle);
			AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(ArmorSet->GetMaxArmorAttribute()).Remove(MaxArmorChangedHandle);
		}
	}

	// Bind armor events to new pawn's armor set
	if (AMGCharacter* Character = Cast<AMGCharacter>(NewPawn))
	{
		Character->OnAbilitySystemReady.AddUniqueDynamic(this, &ThisClass::OnAbilitySystemReady);

		if (UAbilitySystemComponent* AbilitySystemComponent = Character->GetAbilitySystemComponent())
		{
			OnAbilitySystemReady(Character, AbilitySystemComponent);
		}
	}
}

void UMGArmorStatus::OnAbilitySystemReady(AMGCharacter* Character, UAbilitySystemComponent* AbilitySystemComponent)
{
	if (AbilitySystemComponent != nullptr)
	{
		if (const UMGArmorSet* ArmorSet = AbilitySystemComponent->GetSet<UMGArmorSet>())
		{
			ArmorChangedHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(ArmorSet->GetArmorAttribute()).AddUObject(this, &ThisClass::OnArmorChanged);
			MaxArmorChangedHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(ArmorSet->GetMaxArmorAttribute()).AddUObject(this, &ThisClass::OnMaxArmorChanged);

			UpdateArmorStatus(ArmorSet);
		}
	}
}

void UMGArmorStatus::UpdateArmorStatus(const UMGArmorSet* ArmorSet)
{
	FOnAttributeChangeData ArmorAttributeData{};
	ArmorAttributeData.NewValue = ArmorSet->GetArmor();
	OnArmorChanged(ArmorAttributeData);

	FOnAttributeChangeData MaxArmorAttributeData{};
	MaxArmorAttributeData.NewValue = ArmorSet->GetMaxArmor();
	OnMaxArmorChanged(MaxArmorAttributeData);
}

void UMGArmorStatus::OnArmorChanged(const FOnAttributeChangeData& ChangeData)
{
	Armor = ChangeData.NewValue;
	ArmorNormalized = MaxArmor > 0.0f ? Armor / MaxArmor : 0.0f;
	ArmorBar->SetPercent(ArmorNormalized);
	ArmorNumber->SetText(FText::AsNumber(ChangeData.NewValue));
}

void UMGArmorStatus::OnMaxArmorChanged(const FOnAttributeChangeData& ChangeData)
{
	MaxArmor = ChangeData.NewValue;
	ArmorNormalized = MaxArmor > 0.0f ? Armor / MaxArmor : 0.0f;
	ArmorBar->SetPercent(ArmorNormalized);
	MaxArmorNumber->SetText(FText::AsNumber(ChangeData.NewValue));
}
