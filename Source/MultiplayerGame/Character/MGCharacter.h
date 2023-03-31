// Copyright Artem Volovyi. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "MGInputConfig.h"
#include "MGAbilitySystemComponent.h"
#include "MGEquipmentManagerComponent.h"
#include "MGHealthComponent.h"
#include "EnhancedInput/Public/InputMappingContext.h"
#include "MGCharacter.generated.h"

/**
 * AMGCharacter
 *
 *	The base character pawn class used by this project
 */
UCLASS()
class AMGCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:

	AMGCharacter();

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	//~IAbilitySystemInterface interface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	//~End of IAbilitySystemInterface interface

	UAbilitySystemComponent* GetMGAbilitySystemComponent() const;

	void InitPlayer();

	virtual void PossessedBy(AController* NewController) override;

	virtual void OnRep_PlayerState() override;

	void InputAbilityInputTagPressed(FGameplayTag InputTag);
	void InputAbilityInputTagReleased(FGameplayTag InputTag);

	void InputMove(const FInputActionValue& Value);
	void InputLook(const FInputActionValue& Value);
	void InputJump(const FInputActionValue& Value);

	// Begins the death sequence for the character (disables collision, disables movement, etc...)
	UFUNCTION()
	virtual void OnDeathStarted(AActor* OwningActor);

	// Ends the death sequence for the character (detaches controller, destroys pawn, etc...)
	UFUNCTION()
	virtual void OnDeathFinished(AActor* OwningActor);

	// Called when the death sequence for the character has completed
	UFUNCTION(BlueprintImplementableEvent, Meta = (DisplayName = "OnDeathFinished"))
	void K2_OnDeathFinished();

	void DisableMovementAndCollision();
	void UninitializeAndDestroy();
	void UninitializeAbilitySystem();

protected:

	UPROPERTY()
	TObjectPtr<UMGAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UMGEquipmentManagerComponent> EquipmentManagerComponent;

	UPROPERTY()
	TObjectPtr<UMGHealthComponent> HealthComponent;

private:

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UMGInputConfig* InputConfig;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
    UInputMappingContext* InputMapping;
};
