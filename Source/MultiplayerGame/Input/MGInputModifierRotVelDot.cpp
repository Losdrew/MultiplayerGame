// Copyright Artem Volovyi. All Rights Reserved.


#include "Input/MGInputModifierRotVelDot.h"

#include "EnhancedPlayerInput.h"

FInputActionValue UMGInputModifierRotVelDot::ModifyRaw_Implementation(const UEnhancedPlayerInput* PlayerInput, FInputActionValue CurrentValue, float DeltaTime)
{
	if (const APlayerController* PlayerController = PlayerInput->GetOuterAPlayerController())
	{
		if (const APawn* Pawn = PlayerController->GetPawn())
		{
			const FVector PawnVelocityNormalized = Pawn->GetVelocity().GetSafeNormal();
			const FVector ControlForwardVector = PlayerController->GetControlRotation().Vector();
			const float DotProduct = FVector::DotProduct(ControlForwardVector, PawnVelocityNormalized);

			if (DotProduct < 0.0f)
			{
				// The input that should be counter-force to velocity must be a positive value
				return CurrentValue.Get<FVector>() * FVector(-DotProduct);
			}

			return FVector::ZeroVector;
		}
	}
	return CurrentValue;
}
