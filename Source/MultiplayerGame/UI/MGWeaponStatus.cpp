// Copyright Artem Volovyi. All Rights Reserved.


#include "MGWeaponStatus.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "MGEquipmentManagerComponent.h"
#include "MGRangedWeaponInstance.h"

void UMGWeaponStatus::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	APlayerController* PlayerController = GetOwningPlayer();
	PlayerController->OnPossessedPawnChanged.AddUniqueDynamic(this, &ThisClass::OnPossessedPawnChanged);

	if (APawn* PlayerPawn = PlayerController->GetPawn())
	{
		OnPossessedPawnChanged(nullptr, PlayerPawn);
	}
}

void UMGWeaponStatus::OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn)
{
	// Unbind equipped/unequipped events from old pawn's equipment manager component
	if (OldPawn != nullptr)
	{
		if (UMGEquipmentManagerComponent* EquipmentManagerComponent = OldPawn->FindComponentByClass<UMGEquipmentManagerComponent>())
		{
			EquipmentManagerComponent->OnEquipped.RemoveDynamic(this, &ThisClass::OnWeaponEquipped);
			EquipmentManagerComponent->OnUnequipped.RemoveDynamic(this, &ThisClass::OnWeaponUnequipped);
		}
	}

	// Bind equipped/unequipped events to new pawn's equipment manager component
	if (NewPawn != nullptr)
	{
		if (UMGEquipmentManagerComponent* EquipmentManagerComponent = NewPawn->FindComponentByClass<UMGEquipmentManagerComponent>())
		{
			EquipmentManagerComponent->OnEquipped.AddUniqueDynamic(this, &ThisClass::OnWeaponEquipped);
			EquipmentManagerComponent->OnUnequipped.AddUniqueDynamic(this, &ThisClass::OnWeaponUnequipped);

			UpdateWeaponStatus(EquipmentManagerComponent);
		}
	}
}

void UMGWeaponStatus::UpdateWeaponStatus(UMGEquipmentManagerComponent* EquipmentManagerComponent)
{
	if (UMGEquipmentInstance* EquippedWeapon = EquipmentManagerComponent->GetEquippedItem())
	{
		OnWeaponEquipped(EquipmentManagerComponent, EquippedWeapon);
	}
}

void UMGWeaponStatus::OnWeaponEquipped(UMGEquipmentManagerComponent* EquipmentManagerComponent, UMGEquipmentInstance* NewEquipmentInstance)
{
	if (UMGRangedWeaponInstance* EquippedWeapon = Cast<UMGRangedWeaponInstance>(NewEquipmentInstance))
	{
		SetVisibility(ESlateVisibility::Visible);

		WeaponImage->SetBrushFromTexture(EquippedWeapon->EquipmentIcon);
		CurrentAmmoNumber->SetText(FText::AsNumber(EquippedWeapon->GetCurrentAmmo()));
		TotalAmmoNumber->SetText(FText::AsNumber(EquippedWeapon->GetTotalAmmo()));

		EquippedWeapon->OnCurrentAmmoChanged.AddUniqueDynamic(this, &ThisClass::OnCurrentAmmoChanged);
		EquippedWeapon->OnTotalAmmoChanged.AddUniqueDynamic(this, &ThisClass::OnTotalAmmoChanged);
	}
}

void UMGWeaponStatus::OnWeaponUnequipped(UMGEquipmentManagerComponent* EquipmentManagerComponent, UMGEquipmentInstance* OldEquipmentInstance)
{
	if (UMGRangedWeaponInstance* UnequippedWeapon = Cast<UMGRangedWeaponInstance>(OldEquipmentInstance))
	{
		SetVisibility(ESlateVisibility::Collapsed);

		UnequippedWeapon->OnCurrentAmmoChanged.RemoveDynamic(this, &ThisClass::OnCurrentAmmoChanged);
		UnequippedWeapon->OnTotalAmmoChanged.RemoveDynamic(this, &ThisClass::OnTotalAmmoChanged);
	}
}

void UMGWeaponStatus::OnCurrentAmmoChanged(UMGRangedWeaponInstance* WeaponInstance, int NewValue)
{
	CurrentAmmoNumber->SetText(FText::AsNumber(NewValue));
}

void UMGWeaponStatus::OnTotalAmmoChanged(UMGRangedWeaponInstance* WeaponInstance, int NewValue)
{
	TotalAmmoNumber->SetText(FText::AsNumber(NewValue));
}
