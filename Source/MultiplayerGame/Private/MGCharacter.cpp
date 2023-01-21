// Copyright Artem Volovyi. All Rights Reserved.


#include "MGCharacter.h"

#include "MGEnhancedInputComponent.h"
#include "MGGameplayTags.h"
#include "MGPlayerState.h"
#include "EnhancedInputSubsystems.h"

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
	 // Get the player controller
    APlayerController* PC = Cast<APlayerController>(GetController());
 
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

UAbilitySystemComponent* AMGCharacter::GetAbilitySystemComponent() const
{
	return GetPlayerState<AMGPlayerState>()->GetAbilitySystemComponent();
}

void AMGCharacter::InputAbilityInputTagPressed(FGameplayTag InputTag)
{
	if (AMGPlayerState* PS = GetPlayerState<AMGPlayerState>())
	{
		PS->AbilitySystemComponent->AbilityInputTagPressed(InputTag);
	}
}

void AMGCharacter::InputAbilityInputTagReleased(FGameplayTag InputTag)
{
	if (AMGPlayerState* PS = GetPlayerState<AMGPlayerState>())
	{
		PS->AbilitySystemComponent->AbilityInputTagReleased(InputTag);
	}
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