// Copyright Artem Volovyi. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MGPlayerState.h"
#include "GameFramework/PlayerController.h"
#include "MGPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class AMGPlayerController : public APlayerController
{
	GENERATED_BODY()

	AMGPlayerState* GetMGPlayerState() const;

	UMGAbilitySystemComponent* GetMGAbilitySystemComponent() const;

	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;
};
