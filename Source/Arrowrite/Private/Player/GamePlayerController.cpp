// Copyright Epic Games, Inc. All Rights Reserved.

#include "Player/GamePlayerController.h"

#include "Engine/World.h"

void AGamePlayerController::BeginPlay()
{
	Super::BeginPlay();

	NotifyLocalPawnChanged(GetPawn());
}

void AGamePlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	ClearLocalRespawnCountdown(false);

	Super::EndPlay(EndPlayReason);
}

void AGamePlayerController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);

	NotifyLocalPawnChanged(InPawn);
}

void AGamePlayerController::ClientRefreshLocalPawn_Implementation(APawn* NewPawn)
{
	ClearLocalRespawnCountdown(true);
	NotifyLocalPawnChanged(NewPawn ? NewPawn : GetPawn());
}

void AGamePlayerController::ClientNotifyConfirmedHit_Implementation()
{
	if (IsLocalController())
	{
		OnConfirmedHit();
	}
}

void AGamePlayerController::ClientStartRespawnCountdown_Implementation(const FString& KillerName, float RespawnDelay)
{
	if (IsLocalController())
	{
		StartLocalRespawnCountdown(KillerName, RespawnDelay);
	}
}

void AGamePlayerController::SetCrosshairVisible(bool bShouldBeVisible)
{
	const UWorld* World = GetWorld();
	if (!World || World->bIsTearingDown)
	{
		return;
	}

	if (IsLocalController())
	{
		OnCrosshairVisibilityChanged(bShouldBeVisible);
		return;
	}

	ClientSetCrosshairVisible(bShouldBeVisible);
}

void AGamePlayerController::ClientSetCrosshairVisible_Implementation(bool bShouldBeVisible)
{
	const UWorld* World = GetWorld();
	if (!World || World->bIsTearingDown)
	{
		return;
	}

	if (IsLocalController())
	{
		OnCrosshairVisibilityChanged(bShouldBeVisible);
	}
}

float AGamePlayerController::GetRespawnTimeRemaining() const
{
	if (!bRespawnCountdownActive)
	{
		return 0.0f;
	}

	const UWorld* World = GetWorld();
	return World ? FMath::Max(0.0f, RespawnCountdownEndTime - World->GetTimeSeconds()) : 0.0f;
}

float AGamePlayerController::GetRespawnProgressPercent() const
{
	if (!bRespawnCountdownActive || RespawnCountdownTotalTime <= 0.0f)
	{
		return 0.0f;
	}

	return FMath::Clamp(GetRespawnTimeRemaining() / RespawnCountdownTotalTime, 0.0f, 1.0f);
}

int32 AGamePlayerController::GetRespawnSecondsRemaining() const
{
	return FMath::Max(0, FMath::CeilToInt(GetRespawnTimeRemaining()));
}

void AGamePlayerController::NotifyLocalPawnChanged(APawn* NewPawn)
{
	const UWorld* World = GetWorld();
	if (!World || World->bIsTearingDown)
	{
		return;
	}

	if (IsLocalController())
	{
		OnLocalPawnChanged(NewPawn);
		OnCombatHUDVisibilityChanged(NewPawn != nullptr && !bRespawnCountdownActive);
	}
}

void AGamePlayerController::StartLocalRespawnCountdown(const FString& KillerName, float RespawnDelay)
{
	UWorld* World = GetWorld();
	if (!World || World->bIsTearingDown)
	{
		return;
	}

	ActiveRespawnKillerName = KillerName.IsEmpty() ? TEXT("World") : KillerName;
	const float ClampedRespawnDelay = FMath::Max(0.0f, RespawnDelay);
	RespawnCountdownTotalTime = ClampedRespawnDelay;
	RespawnCountdownEndTime = World->GetTimeSeconds() + ClampedRespawnDelay;
	LastRespawnCountdownSecond = INDEX_NONE;
	bRespawnCountdownActive = true;

	OnCombatHUDVisibilityChanged(false);
	OnCrosshairVisibilityChanged(false);
	OnRespawnCountdownStarted(ActiveRespawnKillerName, ClampedRespawnDelay);
	BroadcastRespawnCountdownUpdate();

	if (ClampedRespawnDelay <= 0.0f)
	{
		ClearLocalRespawnCountdown(true);
		return;
	}

	World->GetTimerManager().SetTimer(RespawnCountdownTimerHandle, this, &ThisClass::HandleRespawnCountdownTick, 0.1f, true);
}

void AGamePlayerController::HandleRespawnCountdownTick()
{
	const float RemainingTime = GetRespawnTimeRemaining();
	BroadcastRespawnCountdownUpdate();

	if (RemainingTime <= 0.0f)
	{
		ClearLocalRespawnCountdown(true);
	}
}

void AGamePlayerController::BroadcastRespawnCountdownUpdate()
{
	const float RemainingTime = GetRespawnTimeRemaining();
	OnRespawnCountdownProgressUpdated(
		ActiveRespawnKillerName,
		RemainingTime,
		RespawnCountdownTotalTime,
		GetRespawnProgressPercent());

	const int32 SecondsRemaining = GetRespawnSecondsRemaining();
	if (SecondsRemaining != LastRespawnCountdownSecond)
	{
		LastRespawnCountdownSecond = SecondsRemaining;
		OnRespawnCountdownSecondChanged(ActiveRespawnKillerName, SecondsRemaining);
	}
}

void AGamePlayerController::ClearLocalRespawnCountdown(bool bNotifyFinished)
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(RespawnCountdownTimerHandle);
	}

	const bool bWasRespawning = bRespawnCountdownActive;
	bRespawnCountdownActive = false;
	RespawnCountdownEndTime = 0.0f;
	RespawnCountdownTotalTime = 0.0f;
	LastRespawnCountdownSecond = INDEX_NONE;

	if (bNotifyFinished && bWasRespawning)
	{
		OnRespawnCountdownFinished();
	}
}
