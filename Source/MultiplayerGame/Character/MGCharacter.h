// Copyright Artem Volovyi. All Rights Reserved.

#pragma once

#include "AbilitySystemInterface.h"
#include "MGFirstPersonCharacter.h"

#include "MGCharacter.generated.h"

class UMGInputConfig;
class UMGHealthComponent;
class UInputMappingContext;
class UMGAbilitySystemComponent;
class UMGEquipmentManagerComponent;
class UMGCharacterMovementComponent;
struct FGameplayTag;
struct FInputActionValue;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMGCharacter_PlayerStateReplicated);

/**
 * AMGCharacter
 *
 *	The base character pawn class used by this project
 */
UCLASS()
class AMGCharacter : public AMGFirstPersonCharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:

	AMGCharacter(const FObjectInitializer& ObjectInitializer);

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	//~IAbilitySystemInterface interface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	//~End of IAbilitySystemInterface interface

	UAbilitySystemComponent* GetMGAbilitySystemComponent() const;

	UFUNCTION(BlueprintCallable)
	virtual UMGCharacterMovementComponent* GetMGMovementComponent() const;

	//~AActor interface
	virtual void Reset() override;
	//~End of AActor interface

	//~APawn interface
	virtual void PossessedBy(AController* NewController) override;
	//~End of APawn interface

	void InitPlayer();

	virtual void OnRep_PlayerState() override;

	void InputAbilityInputTagPressed(FGameplayTag InputTag);
	void InputAbilityInputTagReleased(FGameplayTag InputTag);

	void InputMove(const FInputActionValue& Value);
	void InputLook(const FInputActionValue& Value);

	// Begins the death sequence for the character (disables collision, disables movement, etc...)
	UFUNCTION()
	virtual void OnDeathStarted(AActor* OwningActor);

	// Ends the death sequence for the character (detaches controller, destroys pawn, etc...)
	UFUNCTION()
	virtual void OnDeathFinished(AActor* OwningActor);

	// Called when the death sequence for the character has completed
	UFUNCTION(BlueprintImplementableEvent, Meta = (DisplayName = "OnDeathFinished"))
	void K2_OnDeathFinished();

	virtual void Jump() override;

	// Calls Crouch() internally
	UFUNCTION(BlueprintCallable)
	virtual void Slide();

	// Calls UnCrouch() internally
	UFUNCTION(BlueprintCallable)
	virtual void UnSlide();

	UFUNCTION(BlueprintCallable)
	virtual void SetFreeLook(bool NewValue);

protected:

	void DisableMovementAndCollision();
	void DestroyDueToDeath();
	void UninitializeAndDestroy();
	void UninitializeAbilitySystem();

	virtual bool CanJumpInternal_Implementation() const override;

	UFUNCTION()
	void OnRep_bSliding();

public:
	// Set by character movement to specify that this Character is currently sliding
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_bSliding, Category=Character)
	uint32 bSliding : 1;

	// Determines if the Character is in free look mode (movement is independent of camera orientation)
	// If set to true - Character moves at FreeLookMovementDirection (from UMGCharacterMovementComponent)
	// If set to false - Character moves at Controller rotation
	UPROPERTY(BlueprintReadOnly, Category=Character)
	uint32 bFreeLooking : 1;

	// The direction of character movement when free looking
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Replicated, Category=Character)
	FRotator FreeLookMovementDirection;

protected:

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UMGAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UMGEquipmentManagerComponent> EquipmentManagerComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UMGHealthComponent> HealthComponent;

private:

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UMGInputConfig> InputConfig;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
    TObjectPtr<UInputMappingContext> InputMapping;
};
