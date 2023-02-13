// Copyright Artem Volovyi. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "MGInputConfig.h"
#include "MGAbilitySystemComponent.h"
#include "MGEquipmentManagerComponent.h"
#include "EnhancedInput/Public/InputMappingContext.h"
#include "MGCharacter.generated.h"

UCLASS()
class AMGCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMGCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void InitPlayer();

	virtual void PossessedBy(AController* NewController) override;

	virtual void OnRep_PlayerState() override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	void InputAbilityInputTagPressed(FGameplayTag InputTag);

	void InputAbilityInputTagReleased(FGameplayTag InputTag);

	void InputMove(const FInputActionValue& Value);

	void InputLook(const FInputActionValue& Value);

	void InputJump(const FInputActionValue& Value);

protected:

	UPROPERTY()
	TObjectPtr<UMGAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UMGEquipmentManagerComponent> EquipmentManagerComponent;

public:

	UPROPERTY(EditDefaultsOnly, Category = Input)
	UMGInputConfig* InputConfig;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input)
    UInputMappingContext* InputMapping;
};
