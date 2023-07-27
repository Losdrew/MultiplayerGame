// Copyright Artem Volovyi. All Rights Reserved.


#include "MGGameMode.h"

#include "MGAssistSubsystem.h"
#include "MGGameState.h"
#include "MGHealthComponent.h"
#include "MGPlayerState.h"

namespace MatchState
{
	const FName Warmup = FName(TEXT("Warmup"));
	const FName MatchStarting = FName(TEXT("MatchStarting"));
}

void AMGGameMode::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);

	if (const APlayerController* PlayerController = Cast<APlayerController>(NewPlayer))
	{
		FString PlayerName;

		if (PlayerController->IsLocalController())
		{
			PlayerName = TEXT("Server");
		}
		else
		{
			// Client default names should be like Client1, Client2, Client3...
			PlayerName = TEXT("Client") + FString::FromInt(GetNextClientId());
		}

		PlayerController->PlayerState->SetPlayerName(PlayerName);
	}
}

void AMGGameMode::RestartPlayer(AController* NewPlayer)
{
	Super::RestartPlayer(NewPlayer);

	if (const APawn* Pawn = NewPlayer->GetPawn())
	{
		if (UMGHealthComponent* HealthComponent = Pawn->GetComponentByClass<UMGHealthComponent>())
		{
			HealthComponent->OnOwnerKilled.AddUniqueDynamic(this, &ThisClass::OnPlayerKilled);
		}
	}
}

void AMGGameMode::Tick(float DeltaSeconds)
{
	AGameModeBase::Tick(DeltaSeconds);

	if (GetMatchState() == MatchState::WaitingToStart)
	{
		// Check to see if we should start the match
		if (ReadyToStartWarmup())
		{
			UE_LOG(LogGameMode, Log, TEXT("GameMode returned ReadyToStartWarmup"));
			StartWarmup();
		}
	}
	if (GetMatchState() == MatchState::InProgress)
	{
		// Check to see if we should end the match
		if (ReadyToEndMatch())
		{
			UE_LOG(LogGameMode, Log, TEXT("GameMode returned ReadyToEndMatch"));
			EndMatch();
		}
	}
}

bool AMGGameMode::HasMatchStarted() const
{
	bool bHasMatchStarted = Super::HasMatchStarted();

	if (GetMatchState() == MatchState::Warmup || GetMatchState() == MatchState::MatchStarting)
	{
		bHasMatchStarted = false;
	}

	return bHasMatchStarted;
}

void AMGGameMode::OnMatchStateSet()
{
	Super::OnMatchStateSet();

	if (MatchState == MatchState::Warmup)
	{
		HandleWarmupStarted();
	}
	if (MatchState == MatchState::MatchStarting)
	{
		HandleMatchStarting();
	}
}

bool AMGGameMode::ReadyToStartWarmup() const
{
	if (GetMatchState() == MatchState::WaitingToStart)
	{
		if (NumPlayers + NumBots >= MinPlayers)
		{
			return true;
		}
	}
	return false;
}

void AMGGameMode::StartWarmup()
{
	SetMatchState(MatchState::Warmup);
}

void AMGGameMode::SwitchToNextMatchState()
{
	if (MatchState == MatchState::WaitingToStart)
	{
		SetMatchState(MatchState::Warmup);
	}
	else if (MatchState == MatchState::Warmup)
	{
		SetMatchState(MatchState::MatchStarting);
	}
	else if (MatchState == MatchState::MatchStarting)
	{
		SetMatchState(MatchState::InProgress);
	}
	else if (MatchState == MatchState::InProgress)
	{
		SetMatchState(MatchState::WaitingPostMatch);
	}
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

void AMGGameMode::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();

	// Restart all players and reset their stats
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PlayerController = Iterator->Get();

		if (const AMGPlayerState* PlayerState = Cast<AMGPlayerState>(PlayerController->PlayerState))
		{
			PlayerState->GetStatsComponent()->ResetPlayerStats();	
		}

		// Destroys existing pawns so they can be respawned at player start locations
		if (APawn* Pawn = PlayerController->GetPawn())
		{
			Pawn->Reset();
		}

		RestartPlayer(PlayerController);
	}

	GetGameState<AMGGameState>()->SetMatchDuration(MatchDuration);
}

void AMGGameMode::HandleWarmupStarted()
{
	// Restart all players
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PlayerController = Iterator->Get();

		if (PlayerController && (PlayerController->GetPawn() == nullptr) && PlayerCanRestart(PlayerController))
		{
			RestartPlayer(PlayerController);
		}
	}

	// Make sure level streaming is up to date
	GEngine->BlockTillLevelStreamingCompleted(GetWorld());

	// First fire BeginPlay, if we haven't already in waiting to start match
	GetWorldSettings()->NotifyBeginPlay();

	GetGameState<AMGGameState>()->SetMatchDuration(WarmupDuration);
}

void AMGGameMode::HandleMatchStarting()
{
	// Disable input on all players
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PlayerController = Iterator->Get();

		PlayerController->ClientIgnoreLookInput(true);
		PlayerController->ClientIgnoreMoveInput(true);
	}

	GetGameState<AMGGameState>()->SetMatchDuration(MatchStartingDuration);
}

void AMGGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	// If players should start as spectators, leave them in the spectator state
	if (!bStartPlayersAsSpectators && !MustSpectate(NewPlayer))
	{
		// If match is in progress or in warmup, start the player
		if ((IsMatchInProgress() || IsWarmup()) && PlayerCanRestart(NewPlayer))
		{
			RestartPlayer(NewPlayer);
		}
	}
}

bool AMGGameMode::PlayerCanRestart_Implementation(APlayerController* Player)
{
	if (!IsMatchInProgress() && !IsWarmup())
	{
		return false;
	}

	return AGameModeBase::PlayerCanRestart_Implementation(Player);
}

bool AMGGameMode::IsWarmup() const
{
	if (GetMatchState() == MatchState::Warmup)
	{
		return true;
	}

	return false;
}

void AMGGameMode::OnPlayerKilled(AActor* KillerActor, AActor* KilledActor, const FGameplayEffectContextHandle& DamageContext)
{
	// Grant a kill to the killer player
	if (const AMGPlayerState* KillerPlayerState = Cast<AMGPlayerState>(KillerActor))
	{
		KillerPlayerState->GetStatsComponent()->AddPlayerKills();
	}

	UMGAssistSubsystem* AssistSubsystem = GetGameInstance()->GetSubsystem<UMGAssistSubsystem>();

	// Grant an assist to each player who assisted in kill
	for (const APlayerState* AssistPlayer : AssistSubsystem->FindKillAssistPlayers(KillerActor, KilledActor))
	{
		if (const AMGPlayerState* AssistPlayerState = Cast<AMGPlayerState>(AssistPlayer))
		{
			AssistPlayerState->GetStatsComponent()->AddPlayerAssists();
		}
	}

	// Grant a death to the killed player
	if (const AMGPlayerState* KilledPlayerState = Cast<AMGPlayerState>(KilledActor))
	{
		KilledPlayerState->GetStatsComponent()->AddPlayerDeaths();
	}

	// Other players should know about the player who assisted the most
	AActor* AssistActor = AssistSubsystem->FindMaxDamageAssistPlayer(KillerActor, KilledActor);

	GetGameState<AMGGameState>()->MulticastOnPlayerKilled(KillerActor, AssistActor, KilledActor, DamageContext);

	// Damage history must not persist through player deaths
	AssistSubsystem->ClearDamageHistoryForPlayer(KilledActor);

	K2_OnKillScored(KillerActor);
}

int32 AMGGameMode::GetNextClientId()
{
	return ++ClientId;
}
