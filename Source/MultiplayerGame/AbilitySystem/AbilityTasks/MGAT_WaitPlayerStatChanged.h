// Copyright Artem Volovyi. All Rights Reserved.

#pragma once

#include "Abilities/Async/AbilityAsync.h"

#include "MGAT_WaitPlayerStatChanged.generated.h"

class AMGPlayerState;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMGWaitPlayerStatChangedDelegate, int32, Count);

/**
 * 
 */
UCLASS()
class UMGAT_WaitPlayerStatChanged : public UAbilityAsync
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintAssignable)
	FMGWaitPlayerStatChangedDelegate Changed;

	/**
	 * Wait until the specified gameplay tag is Changed in Player State's StatTags container
	 * It will keep listening as long as OnlyTriggerOnce = false.
	 * If OnlyMatchExact = false it will trigger for nested tags
	 * If used in an ability graph, this async action will wait even after activation ends. It's recommended to use WaitGameplayTagAdd instead.
	 */
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (DefaultToSelf = "PlayerState", BlueprintInternalUseOnly = "TRUE"))
	static UMGAT_WaitPlayerStatChanged* WaitPlayerStatChanged(APlayerState* PlayerState, FGameplayTag StatTag, bool bOnlyTriggerOnce=false, bool bOnlyMatchExact=false);

protected:

	virtual void Activate() override;
	virtual void EndAction() override;
	virtual void SetAbilityActor(AActor* InActor) override;

	virtual void GameplayTagCallback(const FGameplayTag Tag, int32 NewCount);

	virtual void BroadcastDelegate(int32 NewStatCount);

	UPROPERTY()
	TObjectPtr<AMGPlayerState> PlayerState;

	FGameplayTag StatTag;

	bool bOnlyTriggerOnce = false;
	bool bOnlyMatchExact = false;

	FDelegateHandle MyHandle;
};
