// Copyright Artem Volovyi. All Rights Reserved.


#include "MGAssetManager.h"

#include "MGGameplayTags.h"

void UMGAssetManager::StartInitialLoading()
{
	UAssetManager::StartInitialLoading();

	UAbilitySystemGlobals::Get().InitGlobalData();

	InitializeAbilitySystem();
}

void UMGAssetManager::InitializeAbilitySystem()
{
	FMGGameplayTags::InitializeNativeTags();

	UAbilitySystemGlobals::Get().InitGlobalData();
}