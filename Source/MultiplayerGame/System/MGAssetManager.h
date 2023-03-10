// Copyright Artem Volovyi. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "AbilitySystemGlobals.h"
#include "MGAssetManager.generated.h"

/**
 * 
 */
UCLASS()
class UMGAssetManager : public UAssetManager
{
	GENERATED_BODY()

	void StartInitialLoading() override;
	void InitializeAbilitySystem();
};