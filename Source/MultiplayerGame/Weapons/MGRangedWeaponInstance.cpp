// Copyright Artem Volovyi. All Rights Reserved.


#include "MGRangedWeaponInstance.h"

#include "MGPhysicalMaterialWithTags.h"
#include "Net/UnrealNetwork.h"
#include "GameplayTagContainer.h"

void UMGRangedWeaponInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UMGRangedWeaponInstance, CurrentAmmo, COND_OwnerOnly, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(UMGRangedWeaponInstance, TotalAmmo, COND_OwnerOnly, REPNOTIFY_OnChanged);
}

void UMGRangedWeaponInstance::OnEquipped()
{
	Super::OnEquipped();
}

void UMGRangedWeaponInstance::OnUnequipped()
{
	Super::OnUnequipped();
}

float UMGRangedWeaponInstance::GetDistanceAttenuation(float Distance) const
{
	const FRichCurve* Curve = DistanceDamageFalloff.GetRichCurveConst();
	return Curve->HasAnyData() ? Curve->Eval(Distance) : 1.0f;
}

float UMGRangedWeaponInstance::GetPhysicalMaterialAttenuation(const UPhysicalMaterial* PhysicalMaterial) const
{
	float Multiplier = 1.0f;

	if (const UMGPhysicalMaterialWithTags* PhysMatWithTags = Cast<const UMGPhysicalMaterialWithTags>(PhysicalMaterial))
	{
		for (const FGameplayTag MaterialTag : PhysMatWithTags->Tags)
		{
			if (const float* PTagMultiplier = MaterialDamageMultiplier.Find(MaterialTag))
			{
				Multiplier *= *PTagMultiplier;
			}
		}
	}

	return Multiplier;
}

void UMGRangedWeaponInstance::OnRep_CurrentAmmo()
{
	OnCurrentAmmoChanged.Broadcast(this, CurrentAmmo);
}

void UMGRangedWeaponInstance::OnRep_TotalAmmo()
{
	OnTotalAmmoChanged.Broadcast(this, TotalAmmo);
}
