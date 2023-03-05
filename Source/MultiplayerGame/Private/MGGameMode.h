// Copyright Artem Volovyi. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "MGGameMode.generated.h"

/**
 * 
 */
UCLASS()
class AMGGameMode : public AGameMode
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable)
	void RequestPlayerRestartNextFrame(AController* Controller, bool bForceReset);
};
