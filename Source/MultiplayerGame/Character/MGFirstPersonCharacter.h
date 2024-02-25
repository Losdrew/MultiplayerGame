#pragma once

#include "GameFramework/Character.h"

#include "MGFirstPersonCharacter.generated.h"

class UCameraComponent;

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

	AMGFirstPersonCharacter(const FObjectInitializer& ObjectInitializer);

	USkeletalMeshComponent* GetFirstPersonArms() const { return FirstPersonArms; }

	virtual void RecalculateBaseEyeHeight() override;

	// Modifies bOwnerNoSee on Mesh
	UFUNCTION(BlueprintCallable)
	void SetMeshOwnerVisibility(bool bVisible);

	// Modifies bOwnerNoSee on FirstPersonBodyMesh
	UFUNCTION(BlueprintCallable)
	void SetLowerBodyOwnerVisibility(bool bVisible);

	// Modifies bOwnerNoSee on FirstPersonArms
	UFUNCTION(BlueprintCallable)
	void SetArmsOwnerVisibility(bool bVisible);

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UCameraComponent> FirstPersonCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<USkeletalMeshComponent> FirstPersonArms;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<USkeletalMeshComponent> FirstPersonBodyMesh;
};
