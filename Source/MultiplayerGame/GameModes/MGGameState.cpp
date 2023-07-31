// Copyright Artem Volovyi. All Rights Reserved.


#include "GameModes/MGGameState.h"

#include "MGAssistSubsystem.h"
#include "Net/UnrealNetwork.h"
#include "MGGameMode.h"

void AMGGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AMGGameState, CurrentMatchDuration);
	DOREPLIFETIME_CONDITION(AMGGameState, CurrentRemainingTime, COND_InitialOnly);
}

void AMGGameState::OnRep_MatchState()
{
	Super::OnRep_MatchState();

	OnMatchStateChanged.Broadcast(MatchState);
}

void AMGGameState::DefaultTimer()
{
	if (CurrentRemainingTime <= 0)
	{
		if (AMGGameMode* GameMode = Cast<AMGGameMode>(AuthorityGameMode))
		{
			GameMode->SwitchToNextMatchState();
		}
		ResetElapsedTime();
	}
	
	if (IsMatchInProgress() || MatchState == MatchState::MatchStarting || MatchState == MatchState::Warmup)
	{
		++ElapsedTime;
		CurrentRemainingTime = CurrentMatchDuration - ElapsedTime;
		if (GetNetMode() != NM_DedicatedServer)
		{
			OnRep_ElapsedTime();
		}
	}

	GetWorldTimerManager().SetTimer(TimerHandle_DefaultTimer, this, &AGameState::DefaultTimer, GetWorldSettings()->GetEffectiveTimeDilation() / GetWorldSettings()->DemoPlayTimeDilation, true);
}

void AMGGameState::ResetElapsedTime()
{
	ElapsedTime = 0;
	OnRep_ElapsedTime();
}

void AMGGameState::HandlePostLogin(AController* Controller)
{
	OnPostLogin.Broadcast(Controller);
}

void AMGGameState::HandleLogout(AController* Controller)
{
	OnLogout.Broadcast(Controller);
}

void AMGGameState::MulticastOnPlayerKilled_Implementation(AActor* KillerActor, AActor* AssistActor, AActor* KilledActor, const FGameplayEffectContextHandle& DamageContext)
{
	OnPlayerKilled.Broadcast(KillerActor, AssistActor, KilledActor, DamageContext);
}
