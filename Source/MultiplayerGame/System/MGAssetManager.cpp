// Copyright Artem Volovyi. All Rights Reserved.


#include "MGAssetManager.h"

void UMGAssetManager::StartInitialLoading()
{
	UAssetManager::StartInitialLoading();

	UAbilitySystemGlobals::Get().InitGlobalData();

	InitializeAbilitySystem();
}

void UMGAssetManager::InitializeAbilitySystem()
{
	UAbilitySystemGlobals::Get().InitGlobalData();
}