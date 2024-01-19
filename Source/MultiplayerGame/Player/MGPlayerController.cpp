// Copyright Artem Volovyi. All Rights Reserved.


#include "MGPlayerController.h"

#include "MGAbilitySystemComponent.h"
#include "MGPlayerState.h"
#include "Kismet/KismetMathLibrary.h"
#include "MGQuickBarComponent.h"

AMGPlayerController::AMGPlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	CameraTiltAngle = 0.0f;
	CameraTiltInterpSpeed = 0.0f;

	QuickBarComponent = CreateDefaultSubobject<UMGQuickBarComponent>(TEXT("QuickBarComponent"));
}

AMGPlayerState* AMGPlayerController::GetMGPlayerState() const
{
	return CastChecked<AMGPlayerState>(PlayerState, ECastCheckedType::NullAllowed);
}

UMGAbilitySystemComponent* AMGPlayerController::GetMGAbilitySystemComponent() const
{
	if (const AMGPlayerState* MGPlayerState = GetMGPlayerState())
	{
		return MGPlayerState->GetMGAbilitySystemComponent();
	}

	return nullptr;
}

void AMGPlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	if (!IsLookInputIgnored())
	{
		if (UMGAbilitySystemComponent* MGASC = GetMGAbilitySystemComponent())
		{
			MGASC->ProcessAbilityInput(DeltaTime, bGamePaused);
		}
	}

	Super::PostProcessInput(DeltaTime, bGamePaused);
}

void AMGPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	TiltCamera(DeltaSeconds);
}

void AMGPlayerController::SetCameraTilt(float TiltAngle, float InterpSpeed)
{
	CameraTiltAngle = TiltAngle;
	CameraTiltInterpSpeed = InterpSpeed;
}

void AMGPlayerController::TiltCamera(float DeltaTime)
{
	const FRotator CurrentRotation = GetControlRotation();
	const FRotator TargetRotation = FRotator(CurrentRotation.Pitch, CurrentRotation.Yaw, CameraTiltAngle);
	SetControlRotation(UKismetMathLibrary::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, CameraTiltInterpSpeed));
}
