// Copyright Artem Volovyi. All Rights Reserved.


#include "MGAnimInstance.h"

#include "AbilitySystemGlobals.h"
#include "MGCharacter.h"
#include "MGCharacterMovementComponent.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

UMGAnimInstance::UMGAnimInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UMGAnimInstance::InitializeWithAbilitySystem(UAbilitySystemComponent* ASC)
{
	if (ASC == nullptr)
	{
		return;
	}

	GameplayTagPropertyMap.Initialize(this, ASC);
}

#if WITH_EDITOR
EDataValidationResult UMGAnimInstance::IsDataValid(FDataValidationContext& Context) const
{
	Super::IsDataValid(Context);

	GameplayTagPropertyMap.IsDataValid(this, Context);

	return ((Context.GetNumErrors() > 0) ? EDataValidationResult::Invalid : EDataValidationResult::Valid);
}
#endif // WITH_EDITOR

void UMGAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	if (AActor* OwningActor = GetOwningActor())
	{
		if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwningActor))
		{
			InitializeWithAbilitySystem(ASC);
		}
	}
}

void UMGAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	const AMGCharacter* Character = Cast<AMGCharacter>(GetOwningActor());

	if (Character == nullptr)
	{
		return;
	}

	UMGCharacterMovementComponent* MovementComponent = Cast<UMGCharacterMovementComponent>(Character->GetCharacterMovement());
	const FMGCharacterGroundInfo& GroundInfo = MovementComponent->GetGroundInfo();
	GroundDistance = GroundInfo.GroundDistance;
}
