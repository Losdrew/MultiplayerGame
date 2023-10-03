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

	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable)
	virtual void OnWeaponEquipped(UMGEquipmentManagerComponent* EquipmentManagerComponent, UMGEquipmentInstance* NewEquipmentInstance);

	UFUNCTION(BlueprintCallable)
	virtual void OnWeaponUnequipped(UMGEquipmentManagerComponent* EquipmentManagerComponent, UMGEquipmentInstance* NewEquipmentInstance);

	UFUNCTION(BlueprintCallable)
	virtual void OnCurrentAmmoChanged(UMGRangedWeaponInstance* WeaponInstance, int NewValue);

	UFUNCTION(BlueprintCallable)
	virtual void OnTotalAmmoChanged(UMGRangedWeaponInstance* WeaponInstance, int NewValue);

public:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> WeaponImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CurrentAmmoNumber;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TotalAmmoNumber;

private:

	UPROPERTY()
	TObjectPtr<UMGRangedWeaponInstance> CurrentWeaponInstance;
};
