// Copyright Artem Volovyi. All Rights Reserved.


#include "MGMovementSet.h"

#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"

UMGMovementSet::UMGMovementSet()
{
	MaxJumpCount = 1;
}

void UMGMovementSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UMGMovementSet, MaxJumpCount, COND_None, REPNOTIFY_Always);
}

void UMGMovementSet::OnRep_MaxJumpCount(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMGMovementSet, MaxJumpCount, OldValue);
}

void UMGMovementSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	if (FGameplayAbilityActorInfo* ActorInfo = GetActorInfo())
	{
		if (ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get()))
		{
			Character->JumpMaxCount = GetMaxJumpCount();
		}
	}
}
