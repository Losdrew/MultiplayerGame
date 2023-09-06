// Copyright Artem Volovyi. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/MGGameplayAbility.h"
#include "AbilitySystem/AbilityTasks/MGAbilityTask_OnTick.h"
#include "MGGameplayAbility_Slide.generated.h"

/**
 * UMGGameplayAbility_Slide
 *
 *	Gameplay ability used for character sliding
 */
UCLASS()
class UMGGameplayAbility_Slide : public UMGGameplayAbility
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void StartSliding();

	UFUNCTION()
	void Slide(float DeltaTime);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool ShouldStopSliding();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void StopSliding();

protected:

	UPROPERTY(BlueprintReadOnly, Transient)
	FVector SlideDirection;

	UPROPERTY(BlueprintReadOnly, Transient)
	float PreviousGroundFriction;

	UPROPERTY(BlueprintReadOnly, Transient)
	bool bReachedSlideSpeed;

	UPROPERTY(BlueprintReadOnly, Transient)
	UMGAbilityTask_OnTick* TickAbilityTask;
};
