// Copyright Artem Volovyi. All Rights Reserved.


#include "MGRangedWeaponInstance.h"

#include "Net/UnrealNetwork.h"

void UMGRangedWeaponInstance::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UMGRangedWeaponInstance, CurrentAmmoInClip, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UMGRangedWeaponInstance, CurrentAmmoTotal, COND_OwnerOnly);
}

void UMGRangedWeaponInstance::OnEquipped()
{
	Super::OnEquipped();
}

void UMGRangedWeaponInstance::OnUnequipped()
{
	Super::OnUnequipped();
}