// Copyright Artem Volovyi. All Rights Reserved.


#include "MGCharacter.h"

#include "MGEnhancedInputComponent.h"
#include "MGGameplayTags.h"
#include "MGPlayerState.h"
#include "EnhancedInputSubsystems.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

AMGCharacter::AMGCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    EquipmentManagerComponent = CreateDefaultSubobject<UMGEquipmentManagerComponent>(TEXT("EquipmentManagerComponent"));

    HealthComponent = CreateDefaultSubobject<UMGHealthComponent>(TEXT("HealthComponent"));
	HealthComponent->OnDeathStarted.AddDynamic(this, &ThisClass::OnDeathStarted);
	HealthComponent->OnDeathFinished.AddDynamic(this, &ThisClass::OnDeathFinished);
}

// Called to bind functionality to input
void AMGCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	 // Get the player controller
	 const APlayerController* PC = Cast<APlayerController>(GetController());
 
    // Get the local player subsystem
    UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());

	// Clear out existing mapping, and add our mapping
    Subsystem->ClearAllMappings();
    Subsystem->AddMappingContext(InputMapping, 0);

	UMGEnhancedInputComponent* EnhancedInputComponent = Cast<UMGEnhancedInputComponent>(PlayerInputComponent);

	check(EnhancedInputComponent);

	const FMGGameplayTags& GameplayTags = FMGGameplayTags::Get();
	TArray<uint32> BindHandles;

	EnhancedInputComponent->BindAbilityActions(InputConfig, this, &ThisClass::InputAbilityInputTagPressed, &ThisClass::InputAbilityInputTagReleased, /*out*/ BindHandles);

	EnhancedInputComponent->BindNativeAction(InputConfig, GameplayTags.InputTag_Move, ETriggerEvent::Triggered, this, &ThisClass::InputMove);
	EnhancedInputComponent->BindNativeAction(InputConfig, GameplayTags.InputTag_Look, ETriggerEvent::Triggered, this, &ThisClass::InputLook);
    EnhancedInputComponent->BindNativeAction(InputConfig, GameplayTags.InputTag_Jump, ETriggerEvent::Triggered, this, &ThisClass::InputJump);
}

UAbilitySystemComponent* AMGCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UAbilitySystemComponent* AMGCharacter::GetMGAbilitySystemComponent() const
{
	return Cast<UMGAbilitySystemComponent>(GetAbilitySystemComponent());
}

void AMGCharacter::InitPlayer()
{
	if (AMGPlayerState* PS = GetPlayerState<AMGPlayerState>())
	{
        AbilitySystemComponent = Cast<UMGAbilitySystemComponent>(PS->GetAbilitySystemComponent());

		PS->GetAbilitySystemComponent()->InitAbilityActorInfo(PS, this);

		HealthComponent->OnKilled.AddDynamic(PS, &AMGPlayerState::OnPlayerPawnKilled);
	}

    HealthComponent->InitializeWithAbilitySystem(AbilitySystemComponent);
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
	OnPlayerStateReplicated.Broadcast();
}

void AMGCharacter::InputAbilityInputTagPressed(FGameplayTag InputTag)
{
	AbilitySystemComponent->AbilityInputTagPressed(InputTag);
}

void AMGCharacter::InputAbilityInputTagReleased(FGameplayTag InputTag)
{
	AbilitySystemComponent->AbilityInputTagReleased(InputTag);
}

void AMGCharacter::InputMove(const FInputActionValue& Value)
{
    if (Controller != nullptr)
    {
        const FVector2D MoveValue = Value.Get<FVector2D>();
        const FRotator MovementRotation(0, Controller->GetControlRotation().Yaw, 0);
 
        // Forward/Backward direction
        if (MoveValue.Y != 0.f)
        {
            // Get forward vector
            const FVector Direction = MovementRotation.RotateVector(FVector::ForwardVector);
 
            AddMovementInput(Direction, MoveValue.Y);
        }
 
        // Right/Left direction
        if (MoveValue.X != 0.f)
        {
            // Get right vector
            const FVector Direction = MovementRotation.RotateVector(FVector::RightVector);
 
            AddMovementInput(Direction, MoveValue.X);
        }
    }
}
 
void AMGCharacter::InputLook(const FInputActionValue& Value)
{
    if (Controller != nullptr)
    {
        const FVector2D LookValue = Value.Get<FVector2D>();
 
        if (LookValue.X != 0.f)
        {
            AddControllerYawInput(LookValue.X);
        }
 
        if (LookValue.Y != 0.f)
        {
            AddControllerPitchInput(LookValue.Y);
        }
    }
}

void AMGCharacter::InputJump(const FInputActionValue& Value)
{
    if (Controller != nullptr)
    {
        Jump();
    }
}

void AMGCharacter::OnDeathStarted(AActor*)
{
	DisableMovementAndCollision();
}

void AMGCharacter::OnDeathFinished(AActor*)
{
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::UninitializeAndDestroy);
}

void AMGCharacter::DisableMovementAndCollision()
{
    if (Controller)
	{
		Controller->SetIgnoreMoveInput(true);
	}

	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	check(CapsuleComp);
	CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CapsuleComp->SetCollisionResponseToAllChannels(ECR_Ignore);

	UCharacterMovementComponent* CharacterMovementComponent = CastChecked<UCharacterMovementComponent>(GetCharacterMovement());
	CharacterMovementComponent->StopMovementImmediately();
	CharacterMovementComponent->DisableMovement();
}

void AMGCharacter::UninitializeAndDestroy()
{
	EquipmentManagerComponent->UninitializeComponent();

	HealthComponent->OnDeathStarted.RemoveDynamic(this, &ThisClass::OnDeathStarted);
	HealthComponent->OnDeathFinished.RemoveDynamic(this, &ThisClass::OnDeathFinished);

	if (AMGPlayerState* PS = GetPlayerState<AMGPlayerState>())
	{
		HealthComponent->OnKilled.RemoveDynamic(PS, &AMGPlayerState::OnPlayerPawnKilled);
	}

	if (GetLocalRole() == ROLE_Authority)
	{
		DetachFromControllerPendingDestroy();
		SetLifeSpan(0.1f);
	}

	// Uninitialize the ASC if we're still the avatar actor (otherwise another pawn already did it when they became the avatar actor)
	if (const UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		if (ASC->GetAvatarActor() == this)
		{
			UninitializeAbilitySystem();
		}
	}

	SetActorHiddenInGame(true);
}

void AMGCharacter::UninitializeAbilitySystem()
{
	if (!AbilitySystemComponent)
	{
		return;
	}

	// Uninitialize the ASC if we're still the avatar actor (otherwise another pawn already did it when they became the avatar actor)
	if (AbilitySystemComponent->GetAvatarActor() == GetOwner())
	{
		FGameplayTagContainer AbilityTypesToIgnore;
		AbilityTypesToIgnore.AddTag(FMGGameplayTags::Get().Ability_Behavior_SurvivesDeath);

		AbilitySystemComponent->CancelAbilities(nullptr, &AbilityTypesToIgnore);
		AbilitySystemComponent->ClearAbilityInput();
		AbilitySystemComponent->RemoveAllGameplayCues();

		if (AbilitySystemComponent->GetOwnerActor() != nullptr)
		{
			AbilitySystemComponent->SetAvatarActor(nullptr);
		}
		else
		{
			// If the ASC doesn't have a valid owner actor, we need to clear all actor info
			AbilitySystemComponent->ClearActorInfo();
		}

		// Make sure the health component is uninitialized too
		HealthComponent->UninitializeFromAbilitySystem();
	}

	AbilitySystemComponent = nullptr;
}