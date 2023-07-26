// Copyright Artem Volovyi. All Rights Reserved.


#include "UI/MGKillMessage.h"

#include "MGGameState.h"
#include "MGPhysicalMaterialWithTags.h"
#include "MGRangedWeaponInstance.h"
#include "NativeGameplayTags.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerState.h"

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Gameplay_Weakspot, "Gameplay.WeakSpot");


void UMGKillMessage::OnPlayerKilled(AActor* KillerActor, AActor* KilledActor, const FGameplayEffectContextHandle& DamageContext)
{
	if (const APlayerState* KillerPlayerState = Cast<APlayerState>(KillerActor))
	{
		KillerName->SetText(FText::FromString(KillerPlayerState->GetPlayerName()));
	}

	if (const APlayerState* KilledPlayerState = Cast<APlayerState>(KilledActor))
	{
		KilledName->SetText(FText::FromString(KilledPlayerState->GetPlayerName()));
	}

	if (const UMGRangedWeaponInstance* Weapon = Cast<UMGRangedWeaponInstance>(DamageContext.GetSourceObject()))
	{
		WeaponImage->SetBrushFromTexture(Weapon->KillFeedIcon);
	}
	else
	{
		// If not killed with a weapon, don't show weapon image
		WeaponImage->SetVisibility(ESlateVisibility::Collapsed);
	}

	KillTypeImage->SetBrushFromTexture(KillIcon);

	// If it was a weak spot kill, set different kill type image
	if (const FHitResult* HitResult = DamageContext.GetHitResult())
	{
		if (const UMGPhysicalMaterialWithTags* PhysicalMaterial = Cast<UMGPhysicalMaterialWithTags>(HitResult->PhysMaterial))
		{
			if (PhysicalMaterial->Tags.HasTagExact(TAG_Gameplay_Weakspot))
			{
				KillTypeImage->SetBrushFromTexture(WeakSpotKillIcon);
			}
		}
	}

	ReceiveOnPlayerKilled(KillerActor, KilledActor, DamageContext);
}
