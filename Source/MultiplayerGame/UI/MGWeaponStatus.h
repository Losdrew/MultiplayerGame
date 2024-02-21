// Copyright Artem Volovyi. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"

#include "MGWeaponStatus.generated.h"

class UImage;
class UTextBlock;
class UMGEquipmentInstance;
class UMGRangedWeaponInstance;
class UMGEquipmentManagerComponent;

/**
 * UMGWeaponStatus
 *
 *	Widget that displays information about a weapon (for example: image, ammo, etc...)
 */
UCLASS()
class UMGWeaponStatus : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION()
	void OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn);

protected:

	virtual void NativeOnInitialized() override;

	UFUNCTION(BlueprintCallable)
	void OnWeaponEquipped(UMGEquipmentManagerComponent* EquipmentManagerComponent, UMGEquipmentInstance* NewEquipmentInstance);

	UFUNCTION(BlueprintCallable)
	void OnWeaponUnequipped(UMGEquipmentManagerComponent* EquipmentManagerComponent, UMGEquipmentInstance* OldEquipmentInstance);

	UFUNCTION(BlueprintCallable)
	void OnCurrentAmmoChanged(UMGRangedWeaponInstance* WeaponInstance, int NewValue);

	UFUNCTION(BlueprintCallable)
	void OnTotalAmmoChanged(UMGRangedWeaponInstance* WeaponInstance, int NewValue);

public:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> WeaponImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CurrentAmmoNumber;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TotalAmmoNumber;
};
