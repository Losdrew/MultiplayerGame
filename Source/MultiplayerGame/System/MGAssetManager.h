// Copyright Artem Volovyi. All Rights Reserved.

#pragma once

#include "Engine/AssetManager.h"

#include "MGAssetManager.generated.h"

/**
 * UMGAssetManager
 *
 *	The base asset manager class used by this project
 */
UCLASS()
class UMGAssetManager : public UAssetManager
{
	GENERATED_BODY()

	void StartInitialLoading() override;
	void InitializeAbilitySystem();
};