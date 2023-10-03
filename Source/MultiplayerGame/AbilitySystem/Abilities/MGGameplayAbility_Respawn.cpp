// Copyright Artem Volovyi. All Rights Reserved.


#include "MGGameplayAbility_Respawn.h"

#include "MGHealthComponent.h"

bool UMGGameplayAbility_Respawn::IsAvatarDeadOrDying() const
{
	bool IsDeadOrDying = false;

	if (const AActor* AvatarActor = GetAvatarActorFromActorInfo())
	{
		if (const UMGHealthComponent* HealthComponent = AvatarActor->FindComponentByClass<UMGHealthComponent>())
		{
			IsDeadOrDying = HealthComponent->IsDeadOrDying();
		}
	}

	return IsDeadOrDying;
}

void UMGGameplayAbility_Respawn::BindDeathListener()
{
	ClearDeathListener();

	// Listen for avatar's EndPlay as fallback
	if (AActor* AvatarActor = GetAvatarActorFromActorInfo())
	{
		LastBoundAvatarActor = AvatarActor;
		LastBoundAvatarActor->OnEndPlay.AddDynamic(this, &ThisClass::OnActorEndPlay);
	}

	// Listen for avatar's death
	if (UMGHealthComponent* HealthComponent = LastBoundAvatarActor->FindComponentByClass<UMGHealthComponent>())
	{
		LastBoundHealthComponent = HealthComponent;
		LastBoundHealthComponent->OnDeathStarted.AddDynamic(this, &ThisClass::OnDeathStarted);
	}
}

void UMGGameplayAbility_Respawn::ClearDeathListener()
{
	if (LastBoundAvatarActor)
	{
		LastBoundAvatarActor->OnEndPlay.RemoveDynamic(this, &ThisClass::OnActorEndPlay);
	}

	if (LastBoundHealthComponent)
	{
		LastBoundHealthComponent->OnDeathStarted.RemoveDynamic(this, &ThisClass::OnDeathStarted);
	}
}
