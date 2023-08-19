// Copyright Artem Volovyi. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MGGameplayAbility.h"
#include "MGHealthComponent.h"
#include "MGGameplayAbility_Respawn.generated.h"

/**
 * UMGGameplayAbility_Respawn
 *
 *	Gameplay ability used for handling respawn
 */
UCLASS()
class UMGGameplayAbility_Respawn : public UMGGameplayAbility
{
	GENERATED_BODY()

protected:

	UFUNCTION(BlueprintCallable)
	bool IsAvatarDeadOrDying() const;

	UFUNCTION(BlueprintCallable)
	void BindDeathListener();

	UFUNCTION(BlueprintCallable)
	void ClearDeathListener();

	UFUNCTION(BlueprintImplementableEvent)
	void OnActorEndPlay(AActor* Actor, EEndPlayReason::Type EndPlayReason);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void OnDeathStarted(AActor* DyingActor);

protected:

	UPROPERTY()
	TObjectPtr<AActor> LastBoundAvatarActor;

	UPROPERTY()
	TObjectPtr<UMGHealthComponent> LastBoundHealthComponent;
};
