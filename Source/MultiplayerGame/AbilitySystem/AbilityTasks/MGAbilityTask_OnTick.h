// Copyright Artem Volovyi. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "MGAbilityTask_OnTick.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTickTaskDelegate, float, DeltaTime);

/**
 * 
 */
UCLASS()
class UMGAbilityTask_OnTick : public UAbilityTask
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintAssignable)
	FOnTickTaskDelegate OnTick;

	UMGAbilityTask_OnTick(const FObjectInitializer& ObjectInitializer);

	// Task that executes an action on every tick
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UMGAbilityTask_OnTick* Tick(UGameplayAbility* OwningAbility);

protected:

	virtual void Activate() override;
	virtual void TickTask(float DeltaTime) override;
 
private:

	float TaskDeltaTime;
};
