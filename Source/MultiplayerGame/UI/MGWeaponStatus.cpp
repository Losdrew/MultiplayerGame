// Copyright Artem Volovyi. All Rights Reserved.


#include "MGWeaponStatus.h"

#include "MGCharacter.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "MGEquipmentManagerComponent.h"
#include "MGRangedWeaponInstance.h"

void UMGWeaponStatus::NativeConstruct()
{
	Super::NativeConstruct();

	if (APlayerController* PlayerController = GetOwningPlayer())
	{
		PlayerController->OnPossessedPawnChanged.AddDynamic(this, &ThisClass::OnPossessedPawnChanged);

		if (APawn* PlayerPawn = PlayerController->GetPawn())
		{
			OnPossessedPawnChanged(nullptr, PlayerPawn);
		}
	}
}

void UMGWeaponStatus::OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn)
{
	// Unbind equipped/unequipped events from old pawn's equipment manager component
	if (const AMGCharacter* PlayerCharacter = Cast<AMGCharacter>(OldPawn))
	{
		if (UMGEquipmentManagerComponent* EquipmentManagerComponent = PlayerCharacter->FindComponentByClass<UMGEquipmentManagerComponent>())
		{
			EquipmentManagerComponent->OnEquipped.RemoveDynamic(this, &ThisClass::OnWeaponEquipped);
			EquipmentManagerComponent->OnUnequipped.RemoveDynamic(this, &ThisClass::OnWeaponUnequipped);
		}
	}

	// Bind equipped/unequipped events to new pawn's equipment manager component
	if (const AMGCharacter* PlayerCharacter = Cast<AMGCharacter>(NewPawn))
	{
		if (UMGEquipmentManagerComponent* EquipmentManagerComponent = PlayerCharacter->FindComponentByClass<UMGEquipmentManagerComponent>())
		{
			EquipmentManagerComponent->OnEquipped.AddUniqueDynamic(this, &ThisClass::OnWeaponEquipped);
			EquipmentManagerComponent->OnUnequipped.AddUniqueDynamic(this, &ThisClass::OnWeaponUnequipped);
		}
	}
}

void UMGWeaponStatus::OnWeaponEquipped(UMGEquipmentManagerComponent* EquipmentManagerComponent, UMGEquipmentInstance* NewEquipmentInstance)
{
	CurrentWeaponInstance = Cast<UMGRangedWeaponInstance>(NewEquipmentInstance);

	if (CurrentWeaponInstance)
	{
		if (!IsVisible())
		{
			SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}

		WeaponImage->SetBrushFromTexture(CurrentWeaponInstance->EquipmentIcon);
		CurrentAmmoNumber->SetText(FText::AsNumber(CurrentWeaponInstance->GetCurrentAmmo()));
		TotalAmmoNumber->SetText(FText::AsNumber(CurrentWeaponInstance->GetTotalAmmo()));

		CurrentWeaponInstance->OnCurrentAmmoChanged.AddUniqueDynamic(this, &ThisClass::OnCurrentAmmoChanged);
		CurrentWeaponInstance->OnTotalAmmoChanged.AddUniqueDynamic(this, &ThisClass::OnTotalAmmoChanged);
	}
}

void UMGWeaponStatus::OnWeaponUnequipped(UMGEquipmentManagerComponent* EquipmentManagerComponent, UMGEquipmentInstance* NewEquipmentInstance)
{
	if (CurrentWeaponInstance)
	{
		if (IsVisible())
		{
			SetVisibility(ESlateVisibility::Hidden);
		}

		CurrentWeaponInstance->OnCurrentAmmoChanged.RemoveDynamic(this, &ThisClass::OnCurrentAmmoChanged);
		CurrentWeaponInstance->OnTotalAmmoChanged.RemoveDynamic(this, &ThisClass::OnTotalAmmoChanged);
		CurrentWeaponInstance = nullptr;
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
