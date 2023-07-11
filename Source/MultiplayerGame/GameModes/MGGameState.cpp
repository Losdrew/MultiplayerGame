// Copyright Artem Volovyi. All Rights Reserved.


#include "GameModes/MGGameState.h"

#include "Net/UnrealNetwork.h"

void AMGGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION(AMGGameState, MatchDuration, COND_InitialOnly);
}

void AMGGameState::OnRep_MatchState()
{
	Super::OnRep_MatchState();

	OnMatchStateChanged.Broadcast(MatchState);
}
