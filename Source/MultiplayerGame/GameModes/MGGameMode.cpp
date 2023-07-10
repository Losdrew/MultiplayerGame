// Copyright Artem Volovyi. All Rights Reserved.


#include "MGGameMode.h"

#include "MGGameState.h"

void AMGGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (AMGGameState* FullGameState = GetGameState<AMGGameState>())
	{
		FullGameState->MatchDuration = MatchDuration;
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
