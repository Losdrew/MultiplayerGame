// Copyright Artem Volovyi. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/MGGameplayAbility.h"
#include "MGSlideGameplayAbility.generated.h"

/**
 * UMGSlideGameplayAbility
 *
 *	Gameplay ability used for character sliding
 */
UCLASS()
class UMGSlideGameplayAbility : public UMGGameplayAbility
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void StartSliding();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Slide();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool ShouldStopSliding();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void StopSliding();

protected:

	UPROPERTY(BlueprintReadOnly, Transient)
	FVector VelocityForwardVector;

	UPROPERTY(BlueprintReadOnly, Transient)
	float PreviousGroundFriction;

	UPROPERTY(BlueprintReadOnly, Transient)
	bool bReachedSlideSpeed;
};
