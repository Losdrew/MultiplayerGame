// Copyright Artem Volovyi. All Rights Reserved.


#include "AbilitySystem/Abilities/MGGameplayAbility_Slide.h"

#include "MGCharacter.h"
#include "MGCharacterMovementComponent.h"

void UMGGameplayAbility_Slide::StartSliding_Implementation()
{
	if (AMGCharacter* Character = GetMGCharacterFromActorInfo())
	{
		Character->Slide();

		// Save the direction of player's velocity when they started sliding
		VelocityForwardVector = Character->GetVelocity().GetSafeNormal();

		if (UMGCharacterMovementComponent* CharacterMovement = Character->GetMGMovementComponent())
		{
			// Save previous ground friction to restore later
			PreviousGroundFriction = CharacterMovement->GroundFriction;

			// Replace current value with sliding friction
			CharacterMovement->GroundFriction = CharacterMovement->SlidingFriction;
		}
	}
}

void UMGGameplayAbility_Slide::Slide_Implementation()
{
	// Keep moving the player in the direction they were facing when they started sliding
	if (AMGCharacter* Character = GetMGCharacterFromActorInfo())
	{
		Character->AddMovementInput(VelocityForwardVector);
	}

	if (ShouldStopSliding())
	{
		StopSliding();
	}
}

bool UMGGameplayAbility_Slide::ShouldStopSliding_Implementation()
{
	if (const UMGCharacterMovementComponent* CharacterMovement = GetMGCharacterFromActorInfo()->GetMGMovementComponent())
	{
		if (bReachedSlideSpeed)
		{
			return CharacterMovement->Velocity.Size() < CharacterMovement->MinSlideSpeed;
		}

		bReachedSlideSpeed = CharacterMovement->Velocity.Size() > CharacterMovement->MinSlideSpeed;
	}
	return false;
}

void UMGGameplayAbility_Slide::StopSliding_Implementation()
{
	if (AMGCharacter* Character = GetMGCharacterFromActorInfo())
	{
		Character->UnSlide();

		bReachedSlideSpeed = false;

		if (UMGCharacterMovementComponent* CharacterMovement = Character->GetMGMovementComponent())
		{
			CharacterMovement->GroundFriction = PreviousGroundFriction;
		}
	}
}