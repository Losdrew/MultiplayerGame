// Copyright Artem Volovyi. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "MGAbilitySystemComponent.h"
#include "MGPlayerState.generated.h"

class UMGAbilitySet;

/**
 * AMGPlayerState
 *
 *	The base player state class used by this project
 */
UCLASS()
class AMGPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:

	AMGPlayerState();

	//~AActor interface
	virtual void PostInitializeComponents() override;
	//~End of AActor interface

	//~IAbilitySystemInterface interface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	//~End of IAbilitySystemInterface interface

	UMGAbilitySystemComponent* GetMGAbilitySystemComponent() const;

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Abilities")
	UMGAbilitySet* AbilitySet;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities", Meta = (AllowPrivateAccess = "true"))
	UMGAbilitySystemComponent* AbilitySystemComponent;
};
