// Copyright Artem Volovyi. All Rights Reserved.


#include "GameModes/MGGameState.h"

#include "Net/UnrealNetwork.h"

void AMGGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMGGameState, MatchDuration);
}
