// Copyright Artem Volovyi. All Rights Reserved.


#include "MGCombatSet.h"

#include "Net/UnrealNetwork.h"

UMGCombatSet::UMGCombatSet()
{
	DamageMultiplier = 1.0f;
	FireRateMultiplier = 1.0f;
	ReloadSpeedMultiplier = 1.0f;
}

void UMGCombatSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UMGCombatSet, DamageMultiplier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMGCombatSet, FireRateMultiplier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMGCombatSet, ReloadSpeedMultiplier, COND_None, REPNOTIFY_Always);
}

void UMGCombatSet::OnRep_DamageMultiplier(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMGCombatSet, DamageMultiplier, OldValue);
}

void UMGCombatSet::OnRep_FireRateMultiplier(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMGCombatSet, FireRateMultiplier, OldValue);
}

void UMGCombatSet::OnRep_ReloadSpeedMultiplier(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMGCombatSet, ReloadSpeedMultiplier, OldValue);
}
