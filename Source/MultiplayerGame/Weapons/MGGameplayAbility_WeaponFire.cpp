// Copyright Artem Volovyi. All Rights Reserved.


#include "MGGameplayAbility_WeaponFire.h"

#include "MGRangedWeaponInstance.h"
#include "NativeGameplayTags.h"
#include "MGCollisionChannels.h"
#include "AbilitySystemComponent.h"

// Weapon fire will be blocked/canceled if the player has this tag
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_WeaponNoFiring, "Ability.Action.Weapon.NoFiring")

UMGGameplayAbility_WeaponFire::UMGGameplayAbility_WeaponFire()
{
	SourceBlockedTags.AddTag(TAG_WeaponNoFiring);
}

bool UMGGameplayAbility_WeaponFire::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	bool bResult = Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);

	if (bResult)
	{
		if (GetWeaponInstance() == nullptr)
		{
			bResult = false;
		}
	}

	return bResult;
}

void UMGGameplayAbility_WeaponFire::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	// Bind target data callback
	UAbilitySystemComponent* AbilityComponent = CurrentActorInfo->AbilitySystemComponent.Get();
	check(AbilityComponent);

	OnTargetDataReadyCallbackDelegateHandle = AbilityComponent->AbilityTargetDataSetDelegate(CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey()).AddUObject(this, &ThisClass::OnTargetDataReadyCallback);

	// Update the last firing time
	UMGRangedWeaponInstance* WeaponData = GetWeaponInstance();
	check(WeaponData);
	WeaponData->UpdateFiringTime();

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

bool UMGGameplayAbility_WeaponFire::CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	bool bCanApplyCost = Super::CheckCost(Handle, ActorInfo, OptionalRelevantTags);

	if (const UMGRangedWeaponInstance* WeaponInstance = GetWeaponInstance())
	{
		bCanApplyCost = WeaponInstance->GetCurrentAmmo() - 1 >= 0;
	}

	return bCanApplyCost;
}

void UMGGameplayAbility_WeaponFire::ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	Super::ApplyCost(Handle, ActorInfo, ActivationInfo);

	if (ActorInfo->IsNetAuthority())
	{
		if (UMGRangedWeaponInstance* WeaponInstance = GetWeaponInstance())
		{
			const int CurrentAmmo = WeaponInstance->GetCurrentAmmo();
			WeaponInstance->SetCurrentAmmo(CurrentAmmo - 1);
		}
	}
}

void UMGGameplayAbility_WeaponFire::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (IsEndAbilityValid(Handle, ActorInfo))
	{
		if (ScopeLockCount > 0)
		{
			WaitingToExecute.Add(FPostLockDelegate::CreateUObject(this, &ThisClass::EndAbility, Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled));
			return;
		}

		UAbilitySystemComponent* AbilityComponent = CurrentActorInfo->AbilitySystemComponent.Get();
		check(AbilityComponent);

		// When ability ends, remove delegate and consume target data
		AbilityComponent->AbilityTargetDataSetDelegate(CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey()).Remove(OnTargetDataReadyCallbackDelegateHandle);
		AbilityComponent->ConsumeClientReplicatedTargetData(CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey());

		Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	}
}

void UMGGameplayAbility_WeaponFire::StartRangedWeaponTargeting()
{
	check(CurrentActorInfo);

	const AActor* AvatarActor = CurrentActorInfo->AvatarActor.Get();
	check(AvatarActor);

	UAbilitySystemComponent* AbilityComponent = CurrentActorInfo->AbilitySystemComponent.Get();
	check(AbilityComponent);

	const AController* Controller = GetControllerFromActorInfo();
	check(Controller);

	FScopedPredictionWindow ScopedPrediction(AbilityComponent, CurrentActivationInfo.GetActivationPredictionKey());

	TArray<FHitResult> FoundHits;
	PerformLocalTargeting(FoundHits);

	FGameplayAbilityTargetDataHandle TargetData;
	if (FoundHits.Num() > 0)
	{
		for (const FHitResult& FoundHit : FoundHits)
		{
			FGameplayAbilityTargetData_SingleTargetHit* NewTargetData = new FGameplayAbilityTargetData_SingleTargetHit();
			NewTargetData->HitResult = FoundHit;
			TargetData.Add(NewTargetData);
		}
	}

	OnTargetDataReadyCallback(TargetData, FGameplayTag());
}

void UMGGameplayAbility_WeaponFire::PerformLocalTargeting(OUT TArray<FHitResult>& OutHits) const
{
	const APawn* AvatarPawn = Cast<APawn>(GetAvatarActorFromActorInfo());
	UMGRangedWeaponInstance* WeaponData = GetWeaponInstance();

	if (AvatarPawn && AvatarPawn->IsLocallyControlled() && WeaponData)
	{
		FRangedWeaponFiringInput InputData;
		InputData.WeaponData = WeaponData;

		const FTransform TargetTransform = GetTargetingTransform(AvatarPawn);
		InputData.AimDirection = TargetTransform.GetUnitAxis(EAxis::X);
		InputData.StartTrace = TargetTransform.GetTranslation();

		TraceBulletsInOneShot(InputData, OutHits);
	}
}

FTransform UMGGameplayAbility_WeaponFire::GetTargetingTransform(const APawn* SourcePawn) const
{
	check(SourcePawn);

	const AController* Controller = SourcePawn->GetController();
	FVector SourceLocation = GetTargetingSourceLocation();

	const double FocalDistance = 1024.0f;

	FVector CameraLocation;
	FVector FocalLocation;
	FRotator CameraRotation;

	if (Controller != nullptr && (TargetingSource == EMGAbilityTargetingSource::CameraTowardsFocus || TargetingSource == EMGAbilityTargetingSource::WeaponTowardsFocus))
	{
		const APlayerController* PlayerController = Cast<APlayerController>(Controller);

		if (PlayerController != nullptr)
		{
			PlayerController->GetPlayerViewPoint(CameraLocation, CameraRotation);
		}
		else
		{
			CameraLocation = SourceLocation;
			CameraRotation = Controller->GetControlRotation();
		}

		// The AI controller doesn't need focal adjustments, so we do it only for the player
		if (PlayerController != nullptr)
		{
			const FVector AimDirection = CameraRotation.Vector().GetSafeNormal();
			FocalLocation = CameraLocation + (AimDirection * FocalDistance);
			CameraLocation = FocalLocation + (((SourceLocation - FocalLocation) | AimDirection) * AimDirection);
		}
	}

	if (TargetingSource == EMGAbilityTargetingSource::WeaponTowardsFocus)
	{
		return FTransform((FocalLocation - SourceLocation).Rotation(), SourceLocation);
	}

	return FTransform(CameraRotation, CameraLocation);
}

FVector UMGGameplayAbility_WeaponFire::GetTargetingSourceLocation() const
{
	// Use Pawn's location as a base
	APawn* const AvatarPawn = Cast<APawn>(GetAvatarActorFromActorInfo());
	check(AvatarPawn);

	const FVector SourceLocation = AvatarPawn->GetActorLocation();
	const FQuat SourceRotation = AvatarPawn->GetActorQuat();

	FVector TargetingSourceLocation = SourceLocation;
	
	const FVector LocalOffset = SourceRotation.RotateVector(TargetingOffset);
	TargetingSourceLocation += LocalOffset;

	return TargetingSourceLocation;
}

void UMGGameplayAbility_WeaponFire::TraceBulletsInOneShot(const FRangedWeaponFiringInput& InputData, OUT TArray<FHitResult>& OutHits) const
{
	UMGRangedWeaponInstance* WeaponData = InputData.WeaponData;
	check(WeaponData);

	const int32 BulletsInOneShot = WeaponData->GetBulletsInOneShot();

	for (int32 BulletIndex = 0; BulletIndex < BulletsInOneShot; BulletIndex++)
	{
		const float BaseSpreadAngle = WeaponData->GetCalculatedSpreadAngle();
		const float SpreadAngleMultiplier = WeaponData->GetCalculatedSpreadAngleMultiplier();
		const float ActualSpreadAngle = BaseSpreadAngle * SpreadAngleMultiplier;

		const float HalfSpreadAngleInRadians = FMath::DegreesToRadians(ActualSpreadAngle * 0.5f);

		const FVector BulletDirection = GetRandomDirectionWithinCone(InputData.AimDirection, HalfSpreadAngleInRadians, WeaponData->GetSpreadExponent());

		const FVector TraceEnd = InputData.StartTrace + BulletDirection * WeaponData->GetMaxDamageRange();

		TArray<FHitResult> AllHits;

		FHitResult Hit = WeaponTrace(InputData.StartTrace, TraceEnd, AllHits);

		if (Hit.GetActor())
		{
			if (AllHits.Num() > 0)
			{
				OutHits.Append(AllHits);
			}
		}

		// Make sure there's always an entry in OutHits so the direction can be used for tracers, etc...
		if (OutHits.Num() == 0)
		{
			if (!Hit.bBlockingHit)
			{
				Hit.Location = TraceEnd;
				Hit.ImpactPoint = TraceEnd;
			}

			OutHits.Add(Hit);
		}
	}
}

FVector UMGGameplayAbility_WeaponFire::GetRandomDirectionWithinCone(const FVector& BaseDirection, const float ConeHalfAngleRad, const float DistributionExponent) const
{
	if (ConeHalfAngleRad <= 0.f)
	{
		return BaseDirection.GetSafeNormal();
	}

	float ConeHalfAngleDegrees = FMath::RadiansToDegrees(ConeHalfAngleRad);
	float DistanceFromCenter = FMath::Pow(FMath::FRand(), DistributionExponent);
	float AngleFromCenter = DistanceFromCenter * ConeHalfAngleDegrees;
	float RandomAngleAround = FMath::FRand() * 360.0f;

	FRotator BaseDirectionRotation = BaseDirection.Rotation();
	FQuat BaseDirectionQuat(BaseDirectionRotation);
	FQuat FromCenterQuat(FRotator(0.0f, AngleFromCenter, 0.0f));
	FQuat RandomAngleQuat(FRotator(0.0f, 0.0, RandomAngleAround));

	FQuat ResultingQuat = BaseDirectionQuat * RandomAngleQuat * FromCenterQuat;
	ResultingQuat.Normalize();

	return ResultingQuat.RotateVector(FVector::ForwardVector);
}

FHitResult UMGGameplayAbility_WeaponFire::WeaponTrace(const FVector& StartTrace, const FVector& EndTrace, OUT TArray<FHitResult>& OutHits) const
{
	FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(WeaponTrace), true, GetAvatarActorFromActorInfo());
	TraceParams.bReturnPhysicalMaterial = true;
	AddAdditionalTraceIgnoreActors(TraceParams);

	const ECollisionChannel TraceChannel = TraceChannel_Weapon;

	TArray<FHitResult> Hits;
	GetWorld()->LineTraceMultiByChannel(Hits, StartTrace, EndTrace, TraceChannel, TraceParams);

	FHitResult Hit(ForceInit);
	if (Hits.Num() > 0)
	{
		// Filter the output list to prevent multiple hits on the same actor;
		// this is to prevent a single bullet dealing damage multiple times to
		// a single actor if using an overlap trace
		for (FHitResult& CurrentHit : Hits)
		{
			auto Predicate = [&CurrentHit](const FHitResult& OtherHit)
				{
					return OtherHit.HitObjectHandle == CurrentHit.HitObjectHandle;
				};

			if (!OutHits.ContainsByPredicate(Predicate))
			{
				OutHits.Add(CurrentHit);
			}
		}

		Hit = OutHits.Last();
	}
	else
	{
		Hit.TraceStart = StartTrace;
		Hit.TraceEnd = EndTrace;
	}

	return Hit;
}

void UMGGameplayAbility_WeaponFire::AddAdditionalTraceIgnoreActors(FCollisionQueryParams& TraceParams) const
{
	if (AActor* Avatar = GetAvatarActorFromActorInfo())
	{
		// Ignore any actors attached to the avatar doing the shooting
		TArray<AActor*> AttachedActors;
		Avatar->GetAttachedActors(AttachedActors);
		TraceParams.AddIgnoredActors(AttachedActors);
	}
}

void UMGGameplayAbility_WeaponFire::OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& InData, FGameplayTag ApplicationTag)
{
	UAbilitySystemComponent* AbilityComponent = CurrentActorInfo->AbilitySystemComponent.Get();
	check(AbilityComponent);

	if (const FGameplayAbilitySpec* AbilitySpec = AbilityComponent->FindAbilitySpecFromHandle(CurrentSpecHandle))
	{
		FScopedPredictionWindow ScopedPrediction(AbilityComponent);

		// Take ownership of the target data to make sure no callbacks into game code invalidate it out from under us
		FGameplayAbilityTargetDataHandle LocalTargetDataHandle(MoveTemp(const_cast<FGameplayAbilityTargetDataHandle&>(InData)));

		const bool bShouldNotifyServer = CurrentActorInfo->IsLocallyControlled() && !CurrentActorInfo->IsNetAuthority();
		if (bShouldNotifyServer)
		{
			AbilityComponent->CallServerSetReplicatedTargetData(CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey(), LocalTargetDataHandle, ApplicationTag, AbilityComponent->ScopedPredictionKey);
		}

		// See if we still have ammo
		if (CommitAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo))
		{
			// We fired the weapon, add spread
			UMGRangedWeaponInstance* WeaponData = GetWeaponInstance();
			check(WeaponData);
			WeaponData->AddSpread();

			// Let the blueprint do stuff like apply effects to the targets
			OnRangedWeaponTargetDataReady(LocalTargetDataHandle);
		}
		else
		{
			K2_EndAbility();
		}

		// We've processed the data
		AbilityComponent->ConsumeClientReplicatedTargetData(CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey());
	}
}
