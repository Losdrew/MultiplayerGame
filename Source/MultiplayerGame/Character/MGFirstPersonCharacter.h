#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include <Camera/CameraComponent.h>
#include "MGFirstPersonCharacter.generated.h"

/**
 * AMGFirstPersonCharacter
 *
 *	Character with first person arms and camera
 */
UCLASS()
class AMGFirstPersonCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	AMGFirstPersonCharacter();

	USkeletalMeshComponent* GetFirstPersonArms() const { return FirstPersonArms; }

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UCameraComponent> FirstPersonCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<USkeletalMeshComponent> FirstPersonArms;
};
