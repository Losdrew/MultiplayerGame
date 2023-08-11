// Copyright Artem Volovyi. All Rights Reserved.


#include "AbilitySystem/AbilityTasks/MGAbilityTask_OnTick.h"

UMGAbilityTask_OnTick::UMGAbilityTask_OnTick(const FObjectInitializer& ObjectInitializer)
{
	bTickingTask = true;
}
 
UMGAbilityTask_OnTick* UMGAbilityTask_OnTick::Tick(UGameplayAbility* OwningAbility)
{
	UMGAbilityTask_OnTick* MyObj = NewAbilityTask<UMGAbilityTask_OnTick>(OwningAbility);
	return MyObj;
}
 
void UMGAbilityTask_OnTick::Activate()
{
	Super::Activate();
}
 
void UMGAbilityTask_OnTick::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);
	OnTick.Broadcast(DeltaTime);
}
