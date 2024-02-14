// Copyright Artem Volovyi. All Rights Reserved.

#pragma once

#include "Abilities/Tasks/AbilityTask.h"

#include "MGAT_WaitFOVChange.generated.h"

class UCameraComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FChangeFOVDelegate);

/**
 * 
 */
UCLASS()
class UMGAT_WaitFOVChange : public UAbilityTask
{
	GENERATED_BODY()
	
public:

	UPROPERTY(BlueprintAssignable)
	FChangeFOVDelegate OnTargetFOVReached;

	UFUNCTION()
	void OnTargetFOVReachedCallback();

	UMGAT_WaitFOVChange(const FObjectInitializer& ObjectInitializer);

	// Change the FOV to the specified value using linear interpolation
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UMGAT_WaitFOVChange* WaitFOVChange(UGameplayAbility* OwningAbility, UCameraComponent* CameraComponent, float TargetFOV, float Duration);

protected:

	virtual void Activate() override;
	virtual void TickTask(float DeltaTime) override;
	virtual void OnDestroy(bool AbilityEnded) override;

private:

	float StartTime;
	float PreviousFOV;
	float TargetFOV;
	float Duration;

	TObjectPtr<UCameraComponent> CameraComponent;
	FDelegateHandle DelegateHandle;
};
