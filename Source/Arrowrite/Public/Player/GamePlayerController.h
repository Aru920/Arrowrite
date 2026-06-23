// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TimerManager.h"
#include "GamePlayerController.generated.h"

UCLASS()
class ARROWRITE_API AGamePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
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

	UFUNCTION(BlueprintPure, Category = "UI|Respawn")
	bool IsRespawnCountdownActive() const { return bRespawnCountdownActive; }

	UFUNCTION(BlueprintPure, Category = "UI|Respawn")
	float GetRespawnTimeRemaining() const;

	UFUNCTION(BlueprintPure, Category = "UI|Respawn")
	float GetRespawnProgressPercent() const;

	UFUNCTION(BlueprintPure, Category = "UI|Respawn")
	int32 GetRespawnSecondsRemaining() const;

	UFUNCTION(BlueprintPure, Category = "UI|Respawn")
	const FString& GetRespawnKillerName() const { return ActiveRespawnKillerName; }

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void OnLocalPawnChanged(APawn* NewPawn);

	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void OnConfirmedHit();

	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void OnRespawnCountdownStarted(const FString& KillerName, float RespawnDelay);

	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void OnRespawnCountdownUpdated(const FString& KillerName, float RemainingTime);

	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void OnRespawnCountdownProgressUpdated(const FString& KillerName, float RemainingTime, float TotalTime, float ProgressPercent);

	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void OnRespawnCountdownSecondChanged(const FString& KillerName, int32 SecondsRemaining);

	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void OnRespawnCountdownFinished();

	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void OnCombatHUDVisibilityChanged(bool bShouldBeVisible);

	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void OnCrosshairVisibilityChanged(bool bShouldBeVisible);

private:
	void NotifyLocalPawnChanged(APawn* NewPawn);
	void StartLocalRespawnCountdown(const FString& KillerName, float RespawnDelay);
	void HandleRespawnCountdownTick();
	void BroadcastRespawnCountdownUpdate();
	void ClearLocalRespawnCountdown(bool bNotifyFinished);

	FTimerHandle RespawnCountdownTimerHandle;
	FString ActiveRespawnKillerName;
	float RespawnCountdownEndTime = 0.0f;
	float RespawnCountdownTotalTime = 0.0f;
	int32 LastRespawnCountdownSecond = INDEX_NONE;
	bool bRespawnCountdownActive = false;
};
