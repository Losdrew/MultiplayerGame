// Copyright Artem Volovyi. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"

#include "MGArmorStatus.generated.h"

class UTextBlock;
class UProgressBar;
class UMGArmorSet;
struct FOnAttributeChangeData;

/**
 * UMGArmorStatus
 *
 *	Widget that displays information about a pawn's armor
 */
UCLASS()
class UMGArmorStatus : public UUserWidget
{
	GENERATED_BODY()
	
protected:

	//~UUserWidget interface
	virtual void NativeOnInitialized() override;
	//~End of UUserWidget interface

	void UpdateArmorStatus(const UMGArmorSet* ArmorSet);

	UFUNCTION()
	void OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn);
	
	UFUNCTION()
	void OnAbilitySystemReady(AMGCharacter* Character, UAbilitySystemComponent* AbilitySystemComponent);

	void OnArmorChanged(const FOnAttributeChangeData& ChangeData);
	void OnMaxArmorChanged(const FOnAttributeChangeData& ChangeData);

public:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> ArmorBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ArmorNumber;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> MaxArmorNumber;

protected:

	UPROPERTY()
	float Armor;

	UPROPERTY()
	float ArmorNormalized;

	UPROPERTY()
	float MaxArmor;

private:

	FDelegateHandle ArmorChangedHandle;
	FDelegateHandle MaxArmorChangedHandle;
};
