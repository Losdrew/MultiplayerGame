// Copyright Artem Volovyi. All Rights Reserved.


#include "MGCharacter.h"
#include "MGPlayerState.h"

// Sets default values
AMGCharacter::AMGCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AMGCharacter::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AMGCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AMGCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AMGCharacter::InitPlayer()
{
	if (AMGPlayerState* PS = GetPlayerState<AMGPlayerState>())
	{
		PS->GetAbilitySystemComponent()->InitAbilityActorInfo(PS, this);
	}
}

void AMGCharacter::PossessedBy(AController * NewController)
{
	Super::PossessedBy(NewController);
	InitPlayer();
}

void AMGCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	InitPlayer();
}