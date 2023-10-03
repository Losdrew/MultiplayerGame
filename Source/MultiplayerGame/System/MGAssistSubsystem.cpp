// Copyright Artem Volovyi. All Rights Reserved.


#include "System/MGAssistSubsystem.h"

#include "GameFramework/PlayerState.h"

void UMGAssistSubsystem::OnPlayerReceiveDamage(AActor* Instigator, AActor* Target, const FGameplayEffectSpec& EffectSpec, float Magnitude)
{
	if (Instigator != Target)
	{
		if (APlayerState* InstigatorPlayerState = Cast<APlayerState>(Instigator))
		{
			if (APlayerState* TargetPlayerState = Cast<APlayerState>(Target))
			{
				FMGPlayerDamageDealt& Damage = DamageHistory.FindOrAdd(TargetPlayerState);
				float& DamageTotalFromInstigator = Damage.AccumulatedDamageByPlayer.FindOrAdd(InstigatorPlayerState);
				DamageTotalFromInstigator += Magnitude;
			}
		}
	}
}

TArray<APlayerState*> UMGAssistSubsystem::FindKillAssistPlayers(AActor* KillerActor, AActor* KilledActor)
{
    TArray<TObjectPtr<APlayerState>> AssistPlayerStates;

    if (APlayerState* KilledPlayerState = Cast<APlayerState>(KilledActor))
    {
        if (const FMGPlayerDamageDealt* DamageOnTarget = DamageHistory.Find(KilledPlayerState))
        {
            DamageOnTarget->AccumulatedDamageByPlayer.GenerateKeyArray(AssistPlayerStates);

            AssistPlayerStates = AssistPlayerStates.FilterByPredicate([KillerActor](APlayerState* AssistPlayerState)
            {
                return AssistPlayerState != KillerActor;
            });
        }
    }

    return AssistPlayerStates;
}

APlayerState* UMGAssistSubsystem::FindMaxDamageAssistPlayer(AActor* KillerActor, AActor* KilledActor)
{
    if (APlayerState* KilledPlayerState = Cast<APlayerState>(KilledActor))
    {
        if (FMGPlayerDamageDealt* DamageOnTarget = DamageHistory.Find(KilledPlayerState))
        {
            APlayerState* AssistPlayerState = nullptr;
            float MaxDamage = -1.0f;

            for (const auto& PlayerDamage : DamageOnTarget->AccumulatedDamageByPlayer)
            {
                if (PlayerDamage.Value > MaxDamage && PlayerDamage.Key != KillerActor)
                {
                    MaxDamage = PlayerDamage.Value;
                    AssistPlayerState = Cast<APlayerState>(PlayerDamage.Key);
                }
            }

            return AssistPlayerState;
        }
    }
    return nullptr;
}

void UMGAssistSubsystem::ClearDamageHistoryForPlayer(AActor* Player)
{
	if (APlayerState* PlayerState = Cast<APlayerState>(Player))
	{
		DamageHistory.Remove(PlayerState);
	}
}
