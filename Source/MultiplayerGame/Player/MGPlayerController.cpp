// Copyright Artem Volovyi. All Rights Reserved.


#include "MGPlayerController.h"

#include "MGAbilitySystemComponent.h"
#include "MGPlayerState.h"

AMGPlayerState* AMGPlayerController::GetMGPlayerState() const
{
	return CastChecked<AMGPlayerState>(PlayerState, ECastCheckedType::NullAllowed);
}

UMGAbilitySystemComponent* AMGPlayerController::GetMGAbilitySystemComponent() const
{
	if (const AMGPlayerState* MGPlayerState = GetMGPlayerState())
	{
		return MGPlayerState->GetMGAbilitySystemComponent();
	}

	return nullptr;
}

void AMGPlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	if (UMGAbilitySystemComponent* MGASC = GetMGAbilitySystemComponent())
	{
		MGASC->ProcessAbilityInput(DeltaTime, bGamePaused);
	}

	Super::PostProcessInput(DeltaTime, bGamePaused);
}
