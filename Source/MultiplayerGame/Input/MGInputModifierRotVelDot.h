// Copyright Artem Volovyi. All Rights Reserved.

#pragma once

#include "InputModifiers.h"

#include "MGInputModifierRotVelDot.generated.h"

/** ControlRotationVelocityDotProduct
	*	Adjusts input based on player control and pawn velocity alignment.
	*	If player moves backward relative to control, inversely scales input.
	*/
UCLASS(NotBlueprintable, MinimalAPI, meta = (DisplayName = "ControlRotationVelocityDotProduct"))
class UMGInputModifierRotVelDot : public UInputModifier
{
	GENERATED_BODY()

protected:
	virtual FInputActionValue ModifyRaw_Implementation(const UEnhancedPlayerInput* PlayerInput, FInputActionValue CurrentValue, float DeltaTime) override;
};
