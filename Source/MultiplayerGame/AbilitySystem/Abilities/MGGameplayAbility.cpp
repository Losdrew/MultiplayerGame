// Copyright Artem Volovyi. All Rights Reserved.


#include "MGGameplayAbility.h"

#include "AbilitySystemComponent.h"
#include "MGPlayerController.h"
#include "MGCharacter.h"

UMGGameplayAbility::UMGGameplayAbility(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

APlayerController* UMGGameplayAbility::GetPlayerControllerFromActorInfo() const
{
	if (CurrentActorInfo)
	{
		return CurrentActorInfo->PlayerController.Get();
	}

	return nullptr;
}

AMGPlayerController* UMGGameplayAbility::GetMGPlayerControllerFromActorInfo() const
{
	if (CurrentActorInfo)
	{
		return Cast<AMGPlayerController>(CurrentActorInfo->PlayerController.Get());
	}

	return nullptr;
}

AMGCharacter* UMGGameplayAbility::GetMGCharacterFromActorInfo() const
{
	if (CurrentActorInfo)
	{
		return Cast<AMGCharacter>(CurrentActorInfo->AvatarActor.Get());
	}

	return nullptr;
}

void UMGGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	K2_OnAbilityAdded();

	TryActivateAbilityOnSpawn(ActorInfo, Spec);
}

void UMGGameplayAbility::TryActivateAbilityOnSpawn(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) const
{
	const bool bIsPredicting = (Spec.ActivationInfo.ActivationMode == EGameplayAbilityActivationMode::Predicting);

	// Try to activate if activation policy is on spawn
	if (ActorInfo && !Spec.IsActive() && !bIsPredicting && ActivationPolicy == EMGAbilityActivationPolicy::OnSpawn)
	{
		UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
		const AActor* AvatarActor = ActorInfo->AvatarActor.Get();

		// If avatar actor is torn off or about to die, don't try to activate until we get the new one
		if (ASC && AvatarActor && !AvatarActor->GetTearOff() && AvatarActor->GetLifeSpan() <= 0.0f)
		{
			const bool bIsLocalExecution = (NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::LocalPredicted) || (NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::LocalOnly);
			const bool bIsServerExecution = (NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::ServerOnly) || (NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::ServerInitiated);

			const bool bClientShouldActivate = ActorInfo->IsLocallyControlled() && bIsLocalExecution;
			const bool bServerShouldActivate = ActorInfo->IsNetAuthority() && bIsServerExecution;

			if (bClientShouldActivate || bServerShouldActivate)
			{
				ASC->TryActivateAbility(Spec.Handle);
			}
		}
	}
}

void UMGGameplayAbility::OnPawnAvatarSet()
{
	K2_OnPawnAvatarSet();
}
