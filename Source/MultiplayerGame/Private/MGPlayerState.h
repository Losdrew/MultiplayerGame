// Copyright Artem Volovyi. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "MGAbilitySet.h"
#include "MGAbilitySystemComponent.h"
#include "MGPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class AMGPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AMGPlayerState();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Abilities, meta = (AllowPrivateAccess = "true"))
	UMGAbilitySystemComponent* AbilitySystemComponent;

	UMGAbilitySystemComponent* GetAbilitySystemComponent() const override
    {
        return AbilitySystemComponent;
    }

	UPROPERTY(EditDefaultsOnly, Category = Abilities)
	UMGAbilitySet* AbilitySet;

	virtual void PostInitializeComponents() override;
};
