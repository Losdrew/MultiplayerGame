// Copyright Artem Volovyi. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "MGAbilitySystemComponent.h"
#include "MGGameplayTagStack.h"
#include "MGPlayerState.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FMGPlayerState_StatTagChanged, FGameplayTag, int32);

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

	UFUNCTION()
	void OnTagStackChanged(FGameplayTag Tag, int32 StackCount);

	// Adds a specified number of stacks to the tag (does nothing if StackCount is below 1)
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Stats")
	void AddStatTagStack(FGameplayTag Tag, int32 StackCount);

	// Removes a specified number of stacks from the tag (does nothing if StackCount is below 1)
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Stats")
	void RemoveStatTagStack(FGameplayTag Tag, int32 StackCount);

	// Returns the stack count of the specified tag (or 0 if the tag is not present)
	UFUNCTION(BlueprintCallable, Category = "Stats")
	int32 GetStatTagStackCount(FGameplayTag Tag) const;

	// Returns true if there is at least one stack of the specified tag
	UFUNCTION(BlueprintCallable, Category = "Stats")
	bool HasStatTag(FGameplayTag Tag) const;

	// Reset all stats by removing all stacks
	UFUNCTION(BlueprintCallable, Category = "Stats")
	void ResetAllStats();

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UMGAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(Replicated)
	FMGGameplayTagStackContainer StatTags;

public:

	FMGPlayerState_StatTagChanged OnStatTagChanged;
};
