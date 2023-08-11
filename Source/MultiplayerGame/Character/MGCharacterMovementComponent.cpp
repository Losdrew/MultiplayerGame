// Copyright Artem Volovyi. All Rights Reserved.


#include "Character/MGCharacterMovementComponent.h"

#include "MGCharacter.h"

void UMGCharacterMovementComponent::Crouch(bool bClientSimulation)
{
	Super::Crouch(bClientSimulation);

	if (AMGCharacter* Character = Cast<AMGCharacter>(CharacterOwner))
	{
		Character->bSliding = Character->bIsCrouched;
	}
}

void UMGCharacterMovementComponent::UnCrouch(bool bClientSimulation)
{
	Super::UnCrouch(bClientSimulation);

	if (AMGCharacter* Character = Cast<AMGCharacter>(CharacterOwner))
	{
		Character->bSliding = Character->bIsCrouched;
	}
}

float UMGCharacterMovementComponent::GetMaxSpeed() const
{
	switch(MovementMode)
	{
	case MOVE_Walking:
	case MOVE_NavWalking:
		return IsSliding() ? MaxSlideSpeed : MaxWalkSpeed;
	default:
		return Super::GetMaxSpeed();
	}
}

float UMGCharacterMovementComponent::GetMaxBrakingDeceleration() const
{
	if (IsSliding())
	{
		return BrakingDecelerationSliding;
	}
	return Super::GetMaxBrakingDeceleration();
}

bool UMGCharacterMovementComponent::CanAttemptJump() const
{
	return IsJumpAllowed() &&
		   (IsMovingOnGround() || IsFalling()); // Falling included for double-jump and non-zero jump hold time, but validated by character.
}

void UMGCharacterMovementComponent::HandleWalkingOffLedge(const FVector& PreviousFloorImpactNormal, const FVector& PreviousFloorContactNormal, const FVector& PreviousLocation, float TimeDelta)
{
	// Apply impulse when sliding off a ledge
	if (IsSliding())
	{
		const FVector ImpulseDirection = Velocity.GetSafeNormal() * LedgeSlideImpulse;

		AddImpulse(ImpulseDirection);
	}

	Super::HandleWalkingOffLedge(PreviousFloorImpactNormal, PreviousFloorContactNormal, PreviousLocation, TimeDelta);
}

bool UMGCharacterMovementComponent::IsSliding() const
{
	if (const AMGCharacter* Character = Cast<AMGCharacter>(CharacterOwner))
	{
		return Character && Character->bSliding;
	}
	return false;
}
