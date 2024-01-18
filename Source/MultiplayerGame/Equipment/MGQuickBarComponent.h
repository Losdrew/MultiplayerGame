// Copyright Artem Volovyi. All Rights Reserved.

#pragma once

#include "Components/ActorComponent.h"
#include "MGQuickBarComponent.generated.h"

class UMGEquipmentInstance;
class UMGEquipmentDefinition;
class UMGEquipmentManagerComponent;

UENUM()
enum EMGQuickSlots
{
	QuickSlot1,
	QuickSlot2,
	QuickSlot3,
	QuickSlot4
};

/**
 * FMGQuickSlotEquipmentDefinition
 *
 *	Associates quick slots with equipment definitions
 */
USTRUCT()
struct FMGQuickSlotEquipmentDefinition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TEnumAsByte<EMGQuickSlots> QuickSlot;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UMGEquipmentDefinition> EquipmentDefinition;
};

/**
 * UMGQuickBarComponent
 *
 *	Component that adds equipment quick slots to the pawn
 */
UCLASS(Blueprintable, meta=(BlueprintSpawnableComponent))
class UMGQuickBarComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UMGQuickBarComponent();

	//~UActorComponent interface
	virtual void BeginPlay() override;
	//~End of UActorComponent interface

	UFUNCTION(BlueprintCallable)
	void CycleActiveSlotForward();

	UFUNCTION(BlueprintCallable)
	void CycleActiveSlotBackward();

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void SetActiveSlotIndex(int32 NewIndex);

	UFUNCTION(BlueprintCallable)
	int32 GetActiveSlotIndex() const { return ActiveSlotIndex; }

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void AddItemToSlot(TSubclassOf<UMGEquipmentDefinition> EquipmentClass);

protected:

	UFUNCTION()
	void OnRep_Slots();

	UFUNCTION()
	void OnRep_ActiveSlotIndex();

private:

	UMGEquipmentManagerComponent* GetEquipmentManager() const;

	// Finds quick slot designated for this equipment definition
	int32 FindQuickSlotByEquipmentDefinition(TSubclassOf<UMGEquipmentDefinition> EquipmentDefinition);

	void EquipItemInActiveSlot();
	void EnequipItemInActiveSlot();

public:

	UPROPERTY(EditDefaultsOnly)
	TArray<FMGQuickSlotEquipmentDefinition> QuickSlotEquipmentDefinitions;
	
private:
	
	UPROPERTY(ReplicatedUsing=OnRep_Slots)
	TArray<TObjectPtr<UMGEquipmentInstance>> Slots;

	UPROPERTY(ReplicatedUsing=OnRep_ActiveSlotIndex)
	int32 ActiveSlotIndex = -1;
};
