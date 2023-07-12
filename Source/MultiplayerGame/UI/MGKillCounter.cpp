// Copyright Artem Volovyi. All Rights Reserved.


#include "UI/MGKillCounter.h"

#include "MGCharacter.h"
#include "MGPlayerState.h"

void UMGKillCounter::NativeOnInitialized()
{
	Super::NativeOnInitialized();

    // Here PlayerState is valid only on the server
	if (AMGPlayerState* PlayerState = Cast<AMGPlayerState>(GetOwningPlayerState()))
    {
        PlayerState->OnKillDeathCountChanged.AddDynamic(this, &UMGKillCounter::UpdateKillCount);
        return;
    }

    // So we wait until it's replicated to the client
    if (AMGCharacter* Character = Cast<AMGCharacter>(GetOwningPlayerPawn()))
    {
	    Character->OnPlayerStateReplicated.AddDynamic(this, &ThisClass::OnPlayerStateReplicated);
    }
}

void UMGKillCounter::OnPlayerStateReplicated()
{
    if (AMGPlayerState* PlayerState = Cast<AMGPlayerState>(GetOwningPlayerState()))
    {
        PlayerState->OnKillDeathCountChanged.AddDynamic(this, &UMGKillCounter::UpdateKillCount);
    }
    
    // We don't need to react to OnPlayerStateReplicated anymore, so we unbind from it
    if (AMGCharacter* Character = Cast<AMGCharacter>(GetOwningPlayerPawn()))
    {
	    Character->OnPlayerStateReplicated.RemoveDynamic(this, &ThisClass::OnPlayerStateReplicated);
    }
}

void UMGKillCounter::UpdateKillCount(int PlayerKills, int PlayerDeaths)
{
    KillCount->SetText(FText::AsNumber(PlayerKills));
}
