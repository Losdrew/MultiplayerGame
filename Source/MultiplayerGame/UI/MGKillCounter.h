// Copyright Artem Volovyi. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "MGKillCounter.generated.h"

/**
 * 
 */
UCLASS()
class UMGKillCounter : public UUserWidget
{
	GENERATED_BODY()

protected:

	virtual void NativeOnInitialized() override;

	UFUNCTION()
    void OnPlayerStateReplicated();

	UFUNCTION()
    void UpdateKillCount(int32 PlayerKills);

public:

	UPROPERTY(meta = (BindWidget))
	UTextBlock* KillCount;
};
