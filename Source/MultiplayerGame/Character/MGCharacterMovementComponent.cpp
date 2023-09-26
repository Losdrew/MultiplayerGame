// Copyright Artem Volovyi. All Rights Reserved.


#include "Character/MGCharacterMovementComponent.h"

#include "MGCharacter.h"
#include "Components/CapsuleComponent.h"

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
				FVector JumpDirection = CurrentWall.ImpactNormal;
				const FVector ControlRotationVector = CharacterOwner->GetController()->GetControlRotation().Vector();

			    if (FVector::DotProduct(CurrentWall.ImpactNormal, ControlRotationVector) > 0)
			    {
			        JumpDirection = ControlRotationVector;
			    }

 				Velocity = (Velocity.GetSafeNormal() + JumpDirection) * WallrunJumpVelocity;
			}

			SetMovementMode(MOVE_Falling);

			bWasWallrunning = false;
			
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
		const ECollisionChannel CollisionChannel = Hit.GetComponent()->GetCollisionObjectType();

		if (CollisionChannel != ECC_WorldStatic)
		{
			return;
		}

		FHitResult WallHit(1.0f);

		FindWall(UpdatedComponent->GetComponentLocation(), WallHit);

		if (WallHit.bBlockingHit)
        {
			const float DotProduct = FVector::DotProduct(WallHit.Normal, CharacterOwner->GetActorForwardVector());

			if (DotProduct > -WallrunThreshold && DotProduct < WallrunThreshold)
			{
				StartWallrunning(WallHit);
				return;
			}
        }
	}

	if (IsWallrunning())
	{
		const float CapsuleRadius = CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleRadius();

		if (!IsWithinEdgeTolerance(UpdatedComponent->GetComponentLocation(), Hit.ImpactPoint, CapsuleRadius))
		{
			if (Hit.bBlockingHit)
			{
				StopWallrunning();
			}
		}
	}
}

void UMGCharacterMovementComponent::ProcessLanded(const FHitResult& Hit, float remainingTime, int32 Iterations)
{
	Super::ProcessLanded(Hit, remainingTime, Iterations);

	bWasWallrunning = false;
}

void UMGCharacterMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);

	if (PreviousCustomMode == MOVE_Wallrunning)
	{
		StopWallrunning();
	}
}

void UMGCharacterMovementComponent::StartWallrunning(const FHitResult& Hit)
{
	if (bWasWallrunning)
	{
		return;
	}

	CurrentWall = Hit;

	FVector WallDirection = FVector::CrossProduct(CurrentWall.Normal, FVector::UpVector).GetSafeNormal();
	GetMGCharacterOwner()->FreeLookMovementDirection = CharacterOwner->GetActorForwardVector().ProjectOnTo(WallDirection).Rotation();

	GetMGCharacterOwner()->SetFreeLook(true);

	// The character should wallrun automatically for a brief moment so that
	// they wouldn't be able to stick to a wall and not move on it at all
	StartAutoWallrun();

	SetMovementMode(MOVE_Custom, MOVE_Wallrunning);
}

void UMGCharacterMovementComponent::StopWallrunning()
{
	GetMGCharacterOwner()->SetFreeLook(false);
	StopAutoWallrun();
	SetMovementMode(MOVE_Falling);
	bWasWallrunning = true;
}

void UMGCharacterMovementComponent::StartAutoWallrun()
{
	GetWorld()->GetTimerManager().SetTimer(AutoWallrunTimerHandle, this, &UMGCharacterMovementComponent::StopAutoWallrun, AutoWallrunDuration, false);
	bAutoWallrunActive = true;
}

void UMGCharacterMovementComponent::StopAutoWallrun()
{
	bAutoWallrunActive = false;
}

void UMGCharacterMovementComponent::FindWall(const FVector& CapsuleLocation, FHitResult& OutWallHit) const
{
	// No collision, no wall...
	if (!HasValidData() || !UpdatedComponent->IsQueryCollisionEnabled())
	{
		OutWallHit = FHitResult();
		return;
	}

	check(CharacterOwner->GetCapsuleComponent());

	FVector LeftVector = FVector::CrossProduct(GetMGCharacterOwner()->FreeLookMovementDirection.Vector(), FVector::DownVector);

	const FVector WallCheckStart = CapsuleLocation;
	FVector WallCheckEnd = WallCheckStart + (LeftVector * 100.f);

	GetWorld()->LineTraceSingleByChannel(OutWallHit, WallCheckStart, WallCheckEnd, ECC_Visibility);

	if (!OutWallHit.bBlockingHit)
	{
		WallCheckEnd = WallCheckStart + (-LeftVector * 100.f);
		GetWorld()->LineTraceSingleByChannel(OutWallHit, WallCheckStart, WallCheckEnd, ECC_Visibility);
	}

	if (!OutWallHit.bBlockingHit)
	{
		WallCheckEnd = WallCheckStart + (Velocity.GetSafeNormal()  * 100.f);
		GetWorld()->LineTraceSingleByChannel(OutWallHit, WallCheckStart, WallCheckEnd, ECC_Visibility);
	}
}

void UMGCharacterMovementComponent::UpdateWallrunRotation(float deltaTime, int32 Iterations)
{
	const FRotator CurrentRotation = UpdatedComponent->GetComponentRotation();

    FRotator WallRotation = CurrentWall.Normal.Rotation();

	// Align character's rotation with wall rotation
	const FRotator NewRotation = FMath::RInterpTo(CurrentRotation, WallRotation, deltaTime, 1.f);

    UpdatedComponent->SetRelativeRotation(NewRotation);

	// Move the character slightly away from the wall (so they don't get stuck) based on the capsule's radius
	const FVector Offset = CurrentWall.Normal * CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleRadius();
	const FVector NewLocation = CurrentWall.Location + FVector(Offset.X, Offset.Y, 0);
	
    UpdatedComponent->SetWorldLocation(NewLocation);
}

bool UMGCharacterMovementComponent::IsWallrunning() const
{
	return (CustomMovementMode == MOVE_Wallrunning) && UpdatedComponent;
}

void UMGCharacterMovementComponent::PhysCustom(float deltaTime, int32 Iterations)
{
	Super::PhysCustom(deltaTime, Iterations);

	if (IsWallrunning())
	{
		PhysWallrunning(deltaTime, Iterations);
	}
}

void UMGCharacterMovementComponent::SimulateMovement(float DeltaSeconds)
{
	if (CustomMovementMode == MOVE_Wallrunning)
	{
		FindWall(UpdatedComponent->GetComponentLocation(), CurrentWall);
	}

	Super::SimulateMovement(DeltaSeconds);
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

		FVector FreeLookMovementDirection = GetMGCharacterOwner()->FreeLookMovementDirection.Vector();

		// Move player automatically while autowallrun is active
		if (bAutoWallrunActive)
		{
			AddInputVector(FreeLookMovementDirection, false);
		}

		if (!HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
		{
			CalcVelocity(deltaTime, WallrunningFriction, false, GetMaxBrakingDeceleration());
		}

		Velocity = Velocity.ProjectOnToNormal(FreeLookMovementDirection);

		// Compute current gravity
		const FVector Gravity(0.f, 0.f, GetGravityZ());
		
		// Apply gravity
		Velocity = NewFallVelocity(Velocity, Gravity, deltaTime);

		ApplyRootMotionToVelocity(timeTick);
		DecayFormerBaseVelocity(timeTick);

		FHitResult Hit(1.f);
		FVector Adjusted = Velocity * deltaTime;

		SafeMoveUpdatedComponent(Adjusted, UpdatedComponent->GetComponentQuat(), true, Hit);

		HandleImpact(Hit, deltaTime, Adjusted);
		SlideAlongSurface(Adjusted, deltaTime, Hit.Normal, Hit, false);

		// Don't check acceleration when autowallrun is active
		if (!bAutoWallrunActive)
		{
			if (Acceleration.IsZero())
			{
				StopWallrunning();
				return;
			}
		}

		float subTimeTickRemaining = timeTick * (1.f - Hit.Time);

		Adjusted = Velocity * timeTick;

		// See if we can convert a normally invalid landing spot (based on the hit result) to a usable one.
		if (!Hit.bStartPenetrating && ShouldCheckForValidLandingSpot(timeTick, Adjusted, Hit))
		{
			const FVector PawnLocation = UpdatedComponent->GetComponentLocation();
			FFindFloorResult FloorResult;
			FindFloor(PawnLocation, FloorResult, false);
			if (FloorResult.IsWalkableFloor() && IsValidLandingSpot(PawnLocation, FloorResult.HitResult))
			{
				StopWallrunning();
				remainingTime += subTimeTickRemaining;
				ProcessLanded(FloorResult.HitResult, remainingTime, Iterations);
				return;
			}
		}

		FindWall(UpdatedComponent->GetComponentLocation(), CurrentWall);

		if (!CurrentWall.bBlockingHit)
        {
			StopWallrunning();
			return;
        }

		UpdateWallrunRotation(deltaTime, Iterations);
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
}

void FSavedMove_MGCharacter::CombineWith(const FSavedMove_Character* OldMove, ACharacter* InCharacter, APlayerController* PC, const FVector& OldStartLocation)
{
	if (UMGCharacterMovementComponent* CharMov = Cast<UMGCharacterMovementComponent>(InCharacter->GetCharacterMovement()))
    {
        CharMov->CurrentWall = ((FSavedMove_MGCharacter*)&OldMove)->StartWall;
    }

	FSavedMove_Character::CombineWith(OldMove, InCharacter, PC, OldStartLocation);
}

void FSavedMove_MGCharacter::SetInitialPosition(ACharacter* Character)
{
	Super::SetInitialPosition(Character);

	if (UMGCharacterMovementComponent* CharMov = Cast<UMGCharacterMovementComponent>(Character->GetMovementComponent()))
    {
        StartWall = CharMov->CurrentWall;
    }
}

//////////////////////////////////////////////////////////////////////
// FNetworkPredictionData_Client_MGCharacter

FNetworkPredictionData_Client_MGCharacter::FNetworkPredictionData_Client_MGCharacter(const UCharacterMovementComponent& ClientMovement)
: Super(ClientMovement)
{
}

FSavedMovePtr FNetworkPredictionData_Client_MGCharacter::AllocateNewMove()
{
    return MakeShared<FSavedMove_MGCharacter>();
}