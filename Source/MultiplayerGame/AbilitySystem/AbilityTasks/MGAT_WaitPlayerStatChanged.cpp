// Copyright Artem Volovyi. All Rights Reserved.


#include "AbilitySystem/AbilityTasks/MGAT_WaitPlayerStatChanged.h"

UMGAT_WaitPlayerStatChanged* UMGAT_WaitPlayerStatChanged::WaitPlayerStatChanged(APlayerState* PlayerState, FGameplayTag StatTag, bool bOnlyTriggerOnce, bool bOnlyMatchExact)
{
	UMGAT_WaitPlayerStatChanged* MyObj = NewObject<UMGAT_WaitPlayerStatChanged>();
	MyObj->SetAbilityActor(PlayerState);
	MyObj->StatTag = StatTag;
	MyObj->bOnlyTriggerOnce = bOnlyTriggerOnce;
	MyObj->bOnlyMatchExact = bOnlyMatchExact;

	return MyObj;
}

void UMGAT_WaitPlayerStatChanged::SetAbilityActor(AActor* InActor)
{
	Super::SetAbilityActor(InActor);

	PlayerState = Cast<AMGPlayerState>(InActor);
}

void UMGAT_WaitPlayerStatChanged::Activate()
{
	Super::Activate();

	if (PlayerState)
	{
		MyHandle = PlayerState->OnStatTagChanged.AddUObject(this, &UMGAT_WaitPlayerStatChanged::GameplayTagCallback);
	}
	else
	{
		EndAction();
	}
}

void UMGAT_WaitPlayerStatChanged::GameplayTagCallback(const FGameplayTag InTag, int32 NewCount)
{
	if ((bOnlyMatchExact && InTag.MatchesTagExact(StatTag)) || (!bOnlyMatchExact && InTag.MatchesTag(StatTag)))
	{
		BroadcastDelegate(NewCount);

		if (bOnlyTriggerOnce)
		{
			EndAction();
		}
	}
}

void UMGAT_WaitPlayerStatChanged::BroadcastDelegate(int32 NewStatCount)
{
	Changed.Broadcast(NewStatCount);
}

void UMGAT_WaitPlayerStatChanged::EndAction()
{
	if (PlayerState && MyHandle.IsValid())
	{
		PlayerState->OnStatTagChanged.Remove(MyHandle);
	}
	Super::EndAction();
}
