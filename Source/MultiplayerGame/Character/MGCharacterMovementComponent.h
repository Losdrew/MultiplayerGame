// Copyright Artem Volovyi. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MGCharacterMovementComponent.generated.h"

UENUM(BlueprintType)
enum ECustomMovementMode
{
	/** None (movement is disabled). */
	MOVE_CustomNone		UMETA(DisplayName="CustomNone"),

	/** Running on walls */
	MOVE_Wallrunning	UMETA(DisplayName="Wallrunning"),
};

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

	friend class FSavedMove_MGCharacter;

	//~UMGCharacterMovementComponent interface
	virtual void Crouch(bool bClientSimulation) override;
	virtual void UnCrouch(bool bClientSimulation) override;
	virtual float GetMaxSpeed() const override;
	virtual float GetMaxBrakingDeceleration() const override;
	virtual bool CanAttemptJump() const override;
	virtual bool DoJump(bool bReplayingMoves) override;
	virtual void HandleWalkingOffLedge(const FVector& PreviousFloorImpactNormal, const FVector& PreviousFloorContactNormal, const FVector& PreviousLocation, float TimeDelta) override;
	virtual float GetGravityZ() const override;
	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;
	//~End of UMGCharacterMovementComponent interface

	UFUNCTION(BlueprintCallable)
	AMGCharacter* GetMGCharacterOwner() const;

	UFUNCTION(BlueprintCallable)
	virtual bool IsSliding() const;

	UFUNCTION(BlueprintCallable)
	virtual bool IsWallrunning() const;

	virtual void StartWallrunning(const FHitResult& Hit);

	virtual void StopWallrunning();

protected:

	//~UMGCharacterMovementComponent interface
	virtual void HandleImpact(const FHitResult& Hit, float TimeSlice, const FVector& MoveDelta) override;
	virtual void ProcessLanded(const FHitResult& Hit, float remainingTime, int32 Iterations) override;
	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;
	virtual void PhysCustom(float deltaTime, int32 Iterations) override;
	virtual void SimulateMovement(float DeltaTime) override;
	//~End of UMGCharacterMovementComponent interface

	virtual void PhysWallrunning(float deltaTime, int32 Iterations);

	// Automatic movement when starting to wallrun
	virtual void StartAutoWallrun();
	virtual void StopAutoWallrun();

	virtual void FindWall(const FVector& CapsuleLocation, FHitResult& OutWallHit) const;

	virtual void UpdateWallrunRotation(float deltaTime, int32 Iterations);

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

	// Dot product value which determines the maximum angle (between character and wall) at which wallrunning can start
	UPROPERTY(Category="Character Movement: Wallrunning", EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0", UIMin="0"))
	float WallrunThreshold;

	// The maximum ground speed when wallrunning
	UPROPERTY(Category="Character Movement: Wallrunning", EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0", UIMin="0"))
	float MaxWallrunSpeed;

	// The friction to apply when wallrunning
	UPROPERTY(Category="Character Movement: Wallrunning", EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0", UIMin="0"))
	float WallrunningFriction;

	// Deceleration when wallrunning and not applying acceleration. This is a constant opposing force that directly lowers velocity by a constant value.
	UPROPERTY(Category="Character Movement: Wallrunning", EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0", UIMin="0"))
	float BrakingDecelerationWallrunning;

	// The gravity scale to apply when wallrunning
	UPROPERTY(Category="Character Movement: Wallrunning", EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0", UIMin="0"))
	float WallrunGravityScale;

	// The velocity of the jump performed while wallrunning
	UPROPERTY(Category="Character Movement: Wallrunning", EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0", UIMin="0"))
	float WallrunJumpVelocity;

public:
	// Information about the wall the Character is running on (updated only during wallrunning).
	UPROPERTY(Category="Character Movement: Wallrunning", VisibleInstanceOnly, BlueprintReadOnly, Transient)
	FHitResult CurrentWall;

	// The duration of automatic movement (doesn't require holding a key) when starting to wallrun
	UPROPERTY(Category="Character Movement: Wallrunning", EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0", UIMin="0"))
	float AutoWallrunDuration;

	// Flag that tells if the player stopped wallrunning recently
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Transient)
	uint32 bWasWallrunning : 1;

protected:

	FTimerHandle AutoWallrunTimerHandle;
	uint32 bAutoWallrunActive : 1;
};


/**
 * FSavedMove_MGCharacter
 *
 *	Saved move on the client that has been sent to the server and might need to be played back.
 */
class FSavedMove_MGCharacter : public FSavedMove_Character
{
	using Super = FSavedMove_Character;

public:

	// Information at the start of the move
	FHitResult StartWall;

    virtual void Clear() override;

	virtual void CombineWith(const FSavedMove_Character* OldMove, ACharacter* InCharacter, APlayerController* PC, const FVector& OldStartLocation) override;

	virtual void SetInitialPosition(ACharacter* Character) override;
};


/**
 * FNetworkPredictionData_Client_MGCharacter
 *
 *	Client prediction data of the move
 */
class FNetworkPredictionData_Client_MGCharacter : public FNetworkPredictionData_Client_Character
{
	using Super = FNetworkPredictionData_Client_Character;

public:

	FNetworkPredictionData_Client_MGCharacter(const UCharacterMovementComponent& ClientMovement);

    virtual FSavedMovePtr AllocateNewMove() override;
};