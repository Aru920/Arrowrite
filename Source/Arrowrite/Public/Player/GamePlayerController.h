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

	UFUNCTION(Client, Reliable)
	void ClientNotifyConfirmedHit();

	UFUNCTION(Client, Reliable)
	void ClientStartRespawnCountdown(const FString& KillerName, float RespawnDelay);

	UFUNCTION(BlueprintCallable, Category = "UI")
	void SetCrosshairVisible(bool bShouldBeVisible);

	UFUNCTION(Client, Reliable)
	void ClientSetCrosshairVisible(bool bShouldBeVisible);

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void OnLocalPawnChanged(APawn* NewPawn);

	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void OnConfirmedHit();

	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void OnRespawnCountdownStarted(const FString& KillerName, float RespawnDelay);

	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void OnCrosshairVisibilityChanged(bool bShouldBeVisible);

private:
	void NotifyLocalPawnChanged(APawn* NewPawn);
};
