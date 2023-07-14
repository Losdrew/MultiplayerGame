// Copyright Artem Volovyi. All Rights Reserved.


#include "MGHealthComponent.h"

#include "MGGameplayTags.h"
#include "Delegates/Delegate.h"
#include "GameFramework/Actor.h"
#include "GameplayEffect.h"

#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UMGHealthComponent::UMGHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);

	AbilitySystemComponent = nullptr;
	HealthSet = nullptr;
	DeathState = EMGDeathState::NotDead;
}

void UMGHealthComponent::InitializeWithAbilitySystem(UMGAbilitySystemComponent* InASC)
{
	const AActor* Owner = GetOwner();
	check(Owner);

	if (AbilitySystemComponent)
	{
		return;
	}

	AbilitySystemComponent = InASC;
	if (!AbilitySystemComponent)
	{
		return;
	}

	HealthSet = AbilitySystemComponent->GetSet<UMGHealthSet>();
	if (!HealthSet)
	{
		return;
	}

	// Register to listen for attribute changes
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(HealthSet->GetHealthAttribute()).AddUObject(this, &ThisClass::HandleHealthChanged);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(HealthSet->GetMaxHealthAttribute()).AddUObject(this, &ThisClass::HandleMaxHealthChanged);
	HealthSet->OnOutOfHealth.AddUObject(this, &ThisClass::HandleOutOfHealth);

	// Reset attributes to default values
	AbilitySystemComponent->SetNumericAttributeBase(UMGHealthSet::GetHealthAttribute(), HealthSet->GetMaxHealth());

	ClearGameplayTags();

	OnHealthChanged.Broadcast(this, HealthSet->GetHealth(), HealthSet->GetHealth(), nullptr);
	OnMaxHealthChanged.Broadcast(this, HealthSet->GetMaxHealth(), HealthSet->GetMaxHealth(), nullptr);
}

void UMGHealthComponent::UninitializeFromAbilitySystem()
{
	ClearGameplayTags();

	if (HealthSet)
	{
		HealthSet->OnOutOfHealth.RemoveAll(this);
	}

	HealthSet = nullptr;
	AbilitySystemComponent = nullptr;
}

float UMGHealthComponent::GetHealth() const
{
	if (HealthSet)
	{
		return HealthSet->GetHealth();
	}

	return 0.0f;
}

float UMGHealthComponent::GetHealthNormalized() const
{
	if (HealthSet)
	{
		const float Health = HealthSet->GetHealth();
		const float MaxHealth = HealthSet->GetMaxHealth();

		if (MaxHealth > 0.0f)
		{
			return Health / MaxHealth;
		}
	}

	return 0.0f;
}

float UMGHealthComponent::GetMaxHealth() const
{
	if (HealthSet)
	{
		return HealthSet->GetMaxHealth();
	}

	return 0.0f;
}

void UMGHealthComponent::StartDeath()
{
	if (DeathState != EMGDeathState::NotDead)
	{
		return;
	}

	DeathState = EMGDeathState::DeathStarted;

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->SetLooseGameplayTagCount(FMGGameplayTags::Get().Status_Death_Dying, 1);
	}

	AActor* Owner = GetOwner();
	check(Owner);

	OnDeathStarted.Broadcast(Owner);

	Owner->ForceNetUpdate();
}

void UMGHealthComponent::FinishDeath()
{
	if (DeathState != EMGDeathState::DeathStarted)
	{
		return;
	}

	DeathState = EMGDeathState::DeathFinished;

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->SetLooseGameplayTagCount(FMGGameplayTags::Get().Status_Death_Dead, 1);
	}

	AActor* Owner = GetOwner();
	check(Owner);

	OnDeathFinished.Broadcast(Owner);

	Owner->ForceNetUpdate();
}

void UMGHealthComponent::OnRep_DeathState(EMGDeathState OldDeathState)
{
	const EMGDeathState NewDeathState = DeathState;

	// Revert the death state for now since we rely on StartDeath and FinishDeath to change it
	DeathState = OldDeathState;

	if (OldDeathState > NewDeathState)
	{
		return;
	}

	if (OldDeathState == EMGDeathState::NotDead)
	{
		if (NewDeathState == EMGDeathState::DeathStarted)
		{
			StartDeath();
		}
		else if (NewDeathState == EMGDeathState::DeathFinished)
		{
			StartDeath();
			FinishDeath();
		}
	}
	else if (OldDeathState == EMGDeathState::DeathStarted)
	{
		if (NewDeathState == EMGDeathState::DeathFinished)
		{
			FinishDeath();
		}
	}
}

void UMGHealthComponent::OnUnregister()
{
	Super::OnUnregister();

	UninitializeFromAbilitySystem();
}

void UMGHealthComponent::ClearGameplayTags()
{
	if (AbilitySystemComponent)
	{
		const FMGGameplayTags& GameplayTags = FMGGameplayTags::Get();

		AbilitySystemComponent->SetLooseGameplayTagCount(GameplayTags.Status_Death_Dying, 0);
		AbilitySystemComponent->SetLooseGameplayTagCount(GameplayTags.Status_Death_Dead, 0);
	}
}

void UMGHealthComponent::HandleHealthChanged(const FOnAttributeChangeData& ChangeData)
{
	OnHealthChanged.Broadcast(this, ChangeData.OldValue, ChangeData.NewValue, nullptr);
}

void UMGHealthComponent::HandleMaxHealthChanged(const FOnAttributeChangeData& ChangeData)
{
	OnMaxHealthChanged.Broadcast(this, ChangeData.OldValue, ChangeData.NewValue, nullptr);
}

void UMGHealthComponent::HandleOutOfHealth(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec& DamageEffectSpec, float DamageMagnitude)
{
	#if WITH_SERVER_CODE
	if (AbilitySystemComponent)
	{
		// Send the "GameplayEvent.Death" gameplay event through the owner's ability system
		// This can be used to trigger a death gameplay ability
		{
			FGameplayEventData Payload;
			Payload.EventTag = FMGGameplayTags::Get().GameplayEvent_Death;
			Payload.Instigator = DamageInstigator;
			Payload.Target = AbilitySystemComponent->GetAvatarActor();
			Payload.OptionalObject = DamageEffectSpec.Def;
			Payload.ContextHandle = DamageEffectSpec.GetEffectContext();
			Payload.InstigatorTags = *DamageEffectSpec.CapturedSourceTags.GetAggregatedTags();
			Payload.TargetTags = *DamageEffectSpec.CapturedTargetTags.GetAggregatedTags();
			Payload.EventMagnitude = DamageMagnitude;

			FScopedPredictionWindow NewScopedWindow(AbilitySystemComponent, true);
			AbilitySystemComponent->HandleGameplayEvent(Payload.EventTag, &Payload);
		}
	}

	OnOwnerKilled.Broadcast(DamageInstigator, AbilitySystemComponent->GetOwnerActor());

	#endif
}

void UMGHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UMGHealthComponent, DeathState);
}
