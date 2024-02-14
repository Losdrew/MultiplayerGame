// Copyright Artem Volovyi. All Rights Reserved.


#include "MGAT_WaitFOVChange.h"

#include "Camera/CameraComponent.h"

UMGAT_WaitFOVChange::UMGAT_WaitFOVChange(const FObjectInitializer& ObjectInitializer)
{
	bTickingTask = true;
}

UMGAT_WaitFOVChange* UMGAT_WaitFOVChange::WaitFOVChange(UGameplayAbility* OwningAbility, UCameraComponent* CameraComponent, float TargetFOV, float Duration)
{
	UMGAT_WaitFOVChange* Task = NewAbilityTask<UMGAT_WaitFOVChange>(OwningAbility);

	Task->CameraComponent = CameraComponent;

	if (Task->CameraComponent != nullptr)
	{
		Task->PreviousFOV = Task->CameraComponent->FieldOfView;
	}

	Task->TargetFOV = TargetFOV;
	Task->Duration = Duration;
	return Task;
}

void UMGAT_WaitFOVChange::Activate()
{
	Super::Activate();

	StartTime = GetWorld()->GetTimeSeconds();
}

void UMGAT_WaitFOVChange::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	if (CameraComponent == nullptr)
	{
		EndTask();
		return;
	}

	float ElapsedTime = GetWorld()->GetTimeSeconds() - StartTime;

	if (ElapsedTime >= Duration)
	{
		CameraComponent->SetFieldOfView(TargetFOV);
		OnTargetFOVReachedCallback();
		return;
	}

	float Alpha = ElapsedTime / Duration;
	float NewFOV = FMath::Lerp(PreviousFOV, TargetFOV, Alpha);

	CameraComponent->SetFieldOfView(NewFOV);
}

void UMGAT_WaitFOVChange::OnTargetFOVReachedCallback()
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnTargetFOVReached.Broadcast();
	}

	EndTask();
}

void UMGAT_WaitFOVChange::OnDestroy(bool AbilityEnded)
{
	if (CameraComponent != nullptr)
	{
		CameraComponent->SetFieldOfView(TargetFOV);
	}

	Super::OnDestroy(AbilityEnded);
}
