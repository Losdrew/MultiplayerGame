// Copyright Artem Volovyi. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MGPlayerController.generated.h"

class AMGPlayerState;
class UMGAbilitySystemComponent;

/**
 * 
 */
UCLASS()
class AMGPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	AMGPlayerState* GetMGPlayerState() const;

	UMGAbilitySystemComponent* GetMGAbilitySystemComponent() const;

	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;
};
