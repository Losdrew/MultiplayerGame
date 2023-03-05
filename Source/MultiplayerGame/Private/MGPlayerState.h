// Copyright Artem Volovyi. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "MGAbilitySystemComponent.h"
#include "MGPlayerState.generated.h"

class UMGAbilitySet;

/**
 * 
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

	UMGAbilitySystemComponent* GetAbilitySystemComponent() const override
    {
        return AbilitySystemComponent;
    }

protected:

	UPROPERTY(EditDefaultsOnly, Category = Abilities)
	UMGAbilitySet* AbilitySet;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Abilities, meta = (AllowPrivateAccess = "true"))
	UMGAbilitySystemComponent* AbilitySystemComponent;
};
