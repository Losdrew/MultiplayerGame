// Copyright Artem Volovyi. All Rights Reserved.


#include "System/MGAssistSubsystem.h"

#include "MGPlayerState.h"
#include "GameFramework/PlayerState.h"

void UMGAssistSubsystem::RecordPlayerDamage(AActor* Instigator, AActor* Target, float Magnitude)
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

void UMGAssistSubsystem::GrantAssistsForPlayerKill(AActor* KillerActor, AActor* KilledActor)
{
	if (APlayerState* KilledPlayerState = Cast<APlayerState>(KilledActor))
	{
		// Grant an assist to each player who damaged the target but wasn't the killer
		if (FMGPlayerDamageDealt* DamageOnTarget = DamageHistory.Find(KilledPlayerState))
		{
			for (const auto& PlayerDamageToKilled : DamageOnTarget->AccumulatedDamageByPlayer)
			{
				if (const AMGPlayerState* AssistPlayerState = Cast<AMGPlayerState>(PlayerDamageToKilled.Key))
				{
					if (AssistPlayerState != KillerActor)
					{
						AssistPlayerState->GetStatsComponent()->AddPlayerAssists();
					}
				}
			}

			// Clear the damage log for the killed player
			DamageHistory.Remove(KilledPlayerState);
		}
	}
}
