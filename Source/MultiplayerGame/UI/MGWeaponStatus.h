// Copyright Artem Volovyi. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MGEquipmentManagerComponent.h"
#include "MGRangedWeaponInstance.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "MGWeaponStatus.generated.h"


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

	UFUNCTION(BlueprintCallable)
	virtual void OnWeaponEquipped(UMGEquipmentManagerComponent* EquipmentManagerComponent, UMGEquipmentInstance* NewEquipmentInstance);

	UFUNCTION(BlueprintCallable)
	virtual void OnWeaponUnequipped(UMGEquipmentManagerComponent* EquipmentManagerComponent, UMGEquipmentInstance* NewEquipmentInstance);

	UFUNCTION(BlueprintCallable)
	virtual void OnCurrentAmmoChanged(UMGRangedWeaponInstance* WeaponInstance, int NewValue);

	UFUNCTION(BlueprintCallable)
	virtual void OnTotalAmmoChanged(UMGRangedWeaponInstance* WeaponInstance, int NewValue);

	virtual void NativeConstruct() override;

public:

	UPROPERTY(meta = (BindWidget))
	UImage* WeaponImage;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CurrentAmmoNumber;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TotalAmmoNumber;

private:

	UPROPERTY()
	UMGRangedWeaponInstance* CurrentWeaponInstance;
};
