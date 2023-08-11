// Copyright Artem Volovyi. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MGCharacterMovementComponent.generated.h"

/**
 * UMGCharacterMovementComponent
 *
 *	The base character movement component class used by this project
 */
UCLASS()
class UMGCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:

	//~UMGCharacterMovementComponent interface
	virtual void Crouch(bool bClientSimulation) override;
	virtual void UnCrouch(bool bClientSimulation) override;
	virtual float GetMaxSpeed() const override;
	virtual float GetMaxBrakingDeceleration() const override;
	virtual bool CanAttemptJump() const override;
	virtual void HandleWalkingOffLedge(const FVector& PreviousFloorImpactNormal, const FVector& PreviousFloorContactNormal, const FVector& PreviousLocation, float TimeDelta) override;
	//~End of UMGCharacterMovementComponent interface

	UFUNCTION(BlueprintCallable)
	virtual bool IsSliding() const;

public:
	// The maximum ground speed when sliding
	UPROPERTY(Category="Character Movement: Sliding", EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0", UIMin="0", ForceUnits="cm/s"))
	float MaxSlideSpeed;

	// The minimum ground speed to continue sliding. If below this speed, the character should stop sliding
	UPROPERTY(Category="Character Movement: Sliding", EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0", UIMin="0", ForceUnits="cm/s"))
	float MinSlideSpeed;

	// The impulse to apply when sliding off a ledge
	UPROPERTY(Category="Character Movement: Sliding", EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0", UIMin="0"))
	float LedgeSlideImpulse;

	// The ground friction to apply when sliding
	UPROPERTY(Category="Character Movement: Sliding", EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0", UIMin="0"))
	float SlidingFriction;

	// Deceleration when sliding and not applying acceleration. This is a constant opposing force that directly lowers velocity by a constant value.
	UPROPERTY(Category="Character Movement: Sliding", EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0", UIMin="0"))
	float BrakingDecelerationSliding;
};
