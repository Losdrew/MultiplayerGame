// Copyright Artem Volovyi. All Rights Reserved.


#include "Character/MGCharacterMovementComponent.h"

#include "MGCharacter.h"

//////////////////////////////////////////////////////////////////////
// UMGCharacterMovementComponent

AMGCharacter* UMGCharacterMovementComponent::GetMGCharacterOwner() const
{
	return Cast<AMGCharacter>(CharacterOwner);
}

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
	switch(CustomMovementMode)
	{
	case MOVE_Wallrunning:
		return MaxWallrunSpeed;
	default:
		switch(MovementMode)
		{
		case MOVE_Walking:
		case MOVE_NavWalking:
			return IsSliding() ? MaxSlideSpeed : MaxWalkSpeed;
		default:
			return Super::GetMaxSpeed();
		}
	}
}

float UMGCharacterMovementComponent::GetMaxBrakingDeceleration() const
{
	switch(CustomMovementMode)
	{
	case MOVE_Wallrunning:
		return BrakingDecelerationWallrunning;
	default:
		switch(MovementMode)
		{
		case MOVE_Walking:
		case MOVE_NavWalking:
			return IsSliding() ? BrakingDecelerationSliding : BrakingDecelerationWalking;
		default:
			return Super::GetMaxBrakingDeceleration();
		}
	}
}

bool UMGCharacterMovementComponent::CanAttemptJump() const
{
	return IsJumpAllowed() &&
		   (IsMovingOnGround() || IsFalling() || IsWallrunning());
}

bool UMGCharacterMovementComponent::DoJump(bool bReplayingMoves)
{
	if (CharacterOwner && CharacterOwner->CanJump())
	{
		// Don't jump if we can't move up/down.
		if (!bConstrainToPlane || FMath::Abs(PlaneConstraintNormal.Z) != 1.f)
		{
			Velocity.Z = FMath::Max<FVector::FReal>(Velocity.Z, JumpZVelocity);

			if (IsWallrunning())
			{
				FVector JumpDirection = CurrentWall.Normal;
				const FVector ControlRotationVector = CharacterOwner->GetController()->GetControlRotation().Vector();

			    if (FVector::DotProduct(CurrentWall.ImpactNormal, ControlRotationVector) > 0)
			    {
			        JumpDirection = ControlRotationVector;
			    }

				Velocity = (Velocity.GetSafeNormal() + JumpDirection) * WallrunJumpVelocity;
			}

			SetMovementMode(MOVE_Falling);
			return true;
		}
	}

	return false;
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

void UMGCharacterMovementComponent::HandleImpact(const FHitResult& Hit, float TimeSlice, const FVector& MoveDelta)
{
	Super::HandleImpact(Hit, TimeSlice, MoveDelta);

	if (IsFalling())
	{
		const float DotProduct = FVector::DotProduct(Hit.ImpactNormal, CharacterOwner->GetActorForwardVector());

		if (DotProduct > -WallrunThreshold && DotProduct < WallrunThreshold)
		{
			StartWallrunning(Hit);
		}
	}
}

void UMGCharacterMovementComponent::StartWallrunning(const FHitResult& Hit)
{
	CurrentWall = Hit;

	FVector WallDirection = FVector::CrossProduct(CurrentWall.ImpactNormal, FVector::UpVector).GetSafeNormal();

	FreeLookMovementDirection = CharacterOwner->GetActorForwardVector().ProjectOnToNormal(WallDirection).Rotation();

	GetMGCharacterOwner()->SetFreeLook(true);

	SetMovementMode(MOVE_Custom, MOVE_Wallrunning);
}

void UMGCharacterMovementComponent::StopWallrunning()
{
	GetMGCharacterOwner()->SetFreeLook(false);

	SetMovementMode(MOVE_Falling);
}

bool UMGCharacterMovementComponent::IsWallrunning() const
{
	return (CustomMovementMode == MOVE_Wallrunning) && UpdatedComponent;
}

void UMGCharacterMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);

	if (PreviousCustomMode == MOVE_Wallrunning)
	{
		StopWallrunning();
	}
}

void UMGCharacterMovementComponent::PhysCustom(float deltaTime, int32 Iterations)
{
	Super::PhysCustom(deltaTime, Iterations);

	if (IsWallrunning())
	{
		PhysWallrunning(deltaTime, Iterations);
	}
}

void UMGCharacterMovementComponent::PhysWallrunning(float deltaTime, int32 Iterations)
{
	float remainingTime = deltaTime;

	while ((remainingTime >= MIN_TICK_TIME) && (Iterations < MaxSimulationIterations))
	{
		Iterations++;
		float timeTick = GetSimulationTimeStep(remainingTime, Iterations);
		remainingTime -= timeTick;

		RestorePreAdditiveRootMotionVelocity();

		if (!HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
		{
			CalcVelocity(deltaTime, WallrunningFriction, false, GetMaxBrakingDeceleration());
		}

		Velocity = Velocity.ProjectOnToNormal(FreeLookMovementDirection.Vector());

		// Compute current gravity
		const FVector Gravity(0.f, 0.f, GetGravityZ());
		
		// Apply gravity
		Velocity = NewFallVelocity(Velocity, Gravity, deltaTime);

		ApplyRootMotionToVelocity(timeTick);
		DecayFormerBaseVelocity(timeTick);

		FHitResult Hit(1.f);
		FVector Adjusted = Velocity * deltaTime;

		SafeMoveUpdatedComponent(Adjusted, UpdatedComponent->GetComponentQuat(), true, Hit);

		if (FMath::IsNearlyZero(Acceleration.SizeSquared()))
		{
			StopWallrunning();
			return;
		}

		float subTimeTickRemaining = timeTick * (1.f - Hit.Time);

		if (Hit.bBlockingHit)
		{
			if (IsValidLandingSpot(UpdatedComponent->GetComponentLocation(), Hit))
			{
				remainingTime += subTimeTickRemaining;
				ProcessLanded(Hit, remainingTime, Iterations);
			}
			StopWallrunning();
			return;
		}

		Adjusted = Velocity * timeTick;

		// See if we can convert a normally invalid landing spot (based on the hit result) to a usable one.
		if (!Hit.bStartPenetrating && ShouldCheckForValidLandingSpot(timeTick, Adjusted, Hit))
		{
			const FVector PawnLocation = UpdatedComponent->GetComponentLocation();
			FFindFloorResult FloorResult;
			FindFloor(PawnLocation, FloorResult, false);
			if (FloorResult.IsWalkableFloor() && IsValidLandingSpot(PawnLocation, FloorResult.HitResult))
			{
				remainingTime += subTimeTickRemaining;
				ProcessLanded(FloorResult.HitResult, remainingTime, Iterations);
				StopWallrunning();
				return;
			}
		}
	}
}

bool UMGCharacterMovementComponent::IsSliding() const
{
	return GetMGCharacterOwner() && GetMGCharacterOwner()->bSliding;
}

float UMGCharacterMovementComponent::GetGravityZ() const
{
	if (IsWallrunning())
	{
		return UPawnMovementComponent::GetGravityZ() * WallrunGravityScale;
	}
	return Super::GetGravityZ();
}

FNetworkPredictionData_Client* UMGCharacterMovementComponent::GetPredictionData_Client() const
{
    if (!ClientPredictionData)
    {
        UMGCharacterMovementComponent* MutableThis = const_cast<UMGCharacterMovementComponent*>(this);
		MutableThis->ClientPredictionData = new FNetworkPredictionData_Client_MGCharacter(*this);
    }

    return ClientPredictionData;
}

//////////////////////////////////////////////////////////////////////
// FSavedMove_MGCharacter

void FSavedMove_MGCharacter::Clear()
{
    Super::Clear();

	StartWall = FHitResult();
	SavedFreeLookMovementDirection = FRotator();
}

void FSavedMove_MGCharacter::CombineWith(const FSavedMove_Character* OldMove, ACharacter* InCharacter, APlayerController* PC, const FVector& OldStartLocation)
{
	if (UMGCharacterMovementComponent* CharMov = Cast<UMGCharacterMovementComponent>(InCharacter->GetCharacterMovement()))
    {
        CharMov->CurrentWall = ((FSavedMove_MGCharacter*)&OldMove)->StartWall;
    }

	FSavedMove_Character::CombineWith(OldMove, InCharacter, PC, OldStartLocation);
}

bool FSavedMove_MGCharacter::CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* Character, float MaxDelta) const
{
	if (const AMGCharacter* MGCharacter = Cast<AMGCharacter>(Character))
	{
		if (MGCharacter->bFreeLooking && SavedFreeLookMovementDirection != ((FSavedMove_MGCharacter*)&NewMove)->SavedFreeLookMovementDirection)
	    {
			return false;
	    }
	}

    return Super::CanCombineWith(NewMove, Character, MaxDelta);
}

void FSavedMove_MGCharacter::SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character& ClientData)
{
    Super::SetMoveFor(Character, InDeltaTime, NewAccel, ClientData);

    if (UMGCharacterMovementComponent* CharMov = Cast<UMGCharacterMovementComponent>(Character->GetCharacterMovement()))
    {
		SavedFreeLookMovementDirection = CharMov->FreeLookMovementDirection;
    }
}

void FSavedMove_MGCharacter::PrepMoveFor(ACharacter* Character)
{
    Super::PrepMoveFor(Character);

    if (UMGCharacterMovementComponent* CharMov = Cast<UMGCharacterMovementComponent>(Character->GetCharacterMovement()))
    {
        CharMov->FreeLookMovementDirection = SavedFreeLookMovementDirection;
    }
}

void FSavedMove_MGCharacter::SetInitialPosition(ACharacter* Character)
{
	Super::SetInitialPosition(Character);

	if (UMGCharacterMovementComponent* CharMov = Cast<UMGCharacterMovementComponent>(Character->GetCharacterMovement()))
    {
        StartWall = CharMov->CurrentWall;
    }
}

//////////////////////////////////////////////////////////////////////
// FSavedMove_MGCharacter

FNetworkPredictionData_Client_MGCharacter::FNetworkPredictionData_Client_MGCharacter(const UCharacterMovementComponent& ClientMovement)
: Super(ClientMovement)
{
}

FSavedMovePtr FNetworkPredictionData_Client_MGCharacter::AllocateNewMove()
{
    return MakeShared<FSavedMove_MGCharacter>();
}