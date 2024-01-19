// Copyright Artem Volovyi. All Rights Reserved.

#pragma once

#include "MGPlayerController.generated.h"

class AMGPlayerState;
class UMGAbilitySystemComponent;
class UMGQuickBarComponent;

/**
 * AMGPlayerController
 *
 *	The base player controller class used by this project
 */
UCLASS()
class AMGPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	AMGPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	AMGPlayerState* GetMGPlayerState() const;

	UMGAbilitySystemComponent* GetMGAbilitySystemComponent() const;

	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;

	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable)
	void SetCameraTilt(float TiltAngle, float InterpSpeed);

	UFUNCTION(BlueprintCallable)
	void TiltCamera(float DeltaTime);

protected:

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UMGQuickBarComponent> QuickBarComponent;

protected:

	UPROPERTY(BlueprintReadOnly, Transient)
	float CameraTiltAngle;

	UPROPERTY(BlueprintReadOnly, Transient)
	float CameraTiltInterpSpeed;
};
