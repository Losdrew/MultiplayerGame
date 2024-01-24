// Copyright Artem Volovyi. All Rights Reserved.


#include "MGAT_WaitInputAutoRelease.h"

#include "AbilitySystemComponent.h"

UMGAT_WaitInputAutoRelease* UMGAT_WaitInputAutoRelease::WaitInputAutoRelease(UGameplayAbility* OwningAbility, float WaitTime, bool bTestAlreadyReleased)
{
	UMGAT_WaitInputAutoRelease* Task = NewAbilityTask<UMGAT_WaitInputAutoRelease>(OwningAbility);
	Task->WaitTime = WaitTime;
	Task->bTestInitialState = bTestAlreadyReleased;
	return Task;
}

void UMGAT_WaitInputAutoRelease::OnReleaseCallback()
{
	float ElapsedTime = GetWorld()->GetTimeSeconds() - StartTime;

	UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
	if (!Ability || !ASC)
	{
		return;
	}

	ASC->AbilityReplicatedEventDelegate(EAbilityGenericReplicatedEvent::InputReleased, GetAbilitySpecHandle(), GetActivationPredictionKey()).Remove(DelegateHandle);

	FScopedPredictionWindow ScopedPrediction(ASC, IsPredictingClient());

	if (IsPredictingClient())
	{
		// Tell the server about this
		ASC->ServerSetReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, GetAbilitySpecHandle(), GetActivationPredictionKey(), ASC->ScopedPredictionKey);
	}
	else
	{
		ASC->ConsumeGenericReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, GetAbilitySpecHandle(), GetActivationPredictionKey());
	}

	// We are done. Kill us so we don't keep getting broadcast messages
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnRelease.Broadcast(ElapsedTime);
	}
	EndTask();
}

void UMGAT_WaitInputAutoRelease::Activate()
{
	StartTime = GetWorld()->GetTimeSeconds();

	UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
	if (ASC && Ability)
	{
		if (bTestInitialState && IsLocallyControlled())
		{
			FGameplayAbilitySpec* Spec = Ability->GetCurrentAbilitySpec();
			if (Spec && !Spec->InputPressed)
			{
				OnReleaseCallback();
				return;
			}
		}

		DelegateHandle = ASC->AbilityReplicatedEventDelegate(EAbilityGenericReplicatedEvent::InputReleased, GetAbilitySpecHandle(), GetActivationPredictionKey()).AddUObject(this, &UMGAT_WaitInputAutoRelease::OnReleaseCallback);
		if (IsForRemoteClient())
		{
			if (!ASC->CallReplicatedEventDelegateIfSet(EAbilityGenericReplicatedEvent::InputReleased, GetAbilitySpecHandle(), GetActivationPredictionKey()))
			{
				SetWaitingOnRemotePlayerData();
			}
		}
	}

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UMGAT_WaitInputAutoRelease::OnTimeFinish, WaitTime, false);
}

void UMGAT_WaitInputAutoRelease::OnTimeFinish()
{
	OnReleaseCallback();
}