// Copyright Artem Volovyi. All Rights Reserved.


#include "MGGameMode.h"

#include "MGGameState.h"
#include "MGHealthComponent.h"
#include "MGPlayerState.h"

void AMGGameMode::InitGameState()
{
	Super::InitGameState();

	if (AMGGameState* FullGameState = GetGameState<AMGGameState>())
	{
		FullGameState->MatchDuration = MatchDuration;
	}
}

void AMGGameMode::RestartPlayer(AController* NewPlayer)
{
	Super::RestartPlayer(NewPlayer);

	if (const APawn* Pawn = NewPlayer->GetPawn())
	{
		if (UMGHealthComponent* HealthComponent = Pawn->GetComponentByClass<UMGHealthComponent>())
		{
			HealthComponent->OnOwnerKilled.AddDynamic(this, &ThisClass::OnPlayerKilled);
		}
	}
}

bool AMGGameMode::ReadyToStartMatch_Implementation()
{
	// If bDelayed Start is set, wait for a manual match start
	if (bDelayedStart)
	{
		return false;
	}

	if (GetMatchState() == MatchState::WaitingToStart)
	{
		if (NumPlayers + NumBots >= MinPlayers)
		{
			return true;
		}
	}
	return false;
}

void AMGGameMode::RequestPlayerRestartNextFrame(AController* Controller, bool bForceReset)
{
	if (bForceReset && (Controller != nullptr))
	{
		Controller->Reset();
	}

	if (APlayerController* PC = Cast<APlayerController>(Controller))
	{
		GetWorldTimerManager().SetTimerForNextTick(PC, &APlayerController::ServerRestartPlayer_Implementation);
	}
}

void AMGGameMode::OnPlayerKilled(AActor* KillerPlayer, AActor* KilledPlayer)
{
	if (AMGPlayerState* KillerPlayerState = Cast<AMGPlayerState>(KillerPlayer))
	{
		KillerPlayerState->AddPlayerKills();
	}

	if (AMGPlayerState* KilledPlayerState = Cast<AMGPlayerState>(KilledPlayer))
	{
		KilledPlayerState->AddPlayerDeaths();
	}

	K2_OnPlayerKilled(KillerPlayer, KilledPlayer);
}
