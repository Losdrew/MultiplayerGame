// Copyright Artem Volovyi. All Rights Reserved.

#pragma once

#include "Abilities/Tasks/AbilityTask.h"

#include "MGAT_WaitInputAutoRelease.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInputAutoReleaseDelegate, float, TimeHeld);

/**
 * 
 */
UCLASS()
class UMGAT_WaitInputAutoRelease : public UAbilityTask
{
	GENERATED_BODY()

	UPROPERTY(BlueprintAssignable)
	FInputAutoReleaseDelegate OnRelease;

	UFUNCTION()
	void OnReleaseCallback();

	virtual void Activate() override;

	/** 
	 * Wait until the user releases the input button for this ability's activation. 
	 * Returns time from hitting this node, till release. Will return 0 if input was already released.
	 * If the input is not released within the specified wait time, it is released automatically. 
	*/
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UMGAT_WaitInputAutoRelease* WaitInputAutoRelease(UGameplayAbility* OwningAbility, float WaitTime, bool bTestAlreadyReleased = false);

private:

	void OnTimeFinish();

	float StartTime;
	float WaitTime;
	bool bTestInitialState;
	FDelegateHandle DelegateHandle;
};
