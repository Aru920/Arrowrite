// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GamePlayerController.generated.h"

UCLASS()
class ARROWRITE_API AGamePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void SetPawn(APawn* InPawn) override;

	UFUNCTION(Client, Reliable)
	void ClientRefreshLocalPawn(APawn* NewPawn);

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void OnLocalPawnChanged(APawn* NewPawn);

private:
	void NotifyLocalPawnChanged(APawn* NewPawn);
};
