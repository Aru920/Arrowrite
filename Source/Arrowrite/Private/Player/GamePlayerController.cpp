// Copyright Epic Games, Inc. All Rights Reserved.

#include "Player/GamePlayerController.h"

#include "Engine/World.h"

void AGamePlayerController::BeginPlay()
{
	Super::BeginPlay();

	NotifyLocalPawnChanged(GetPawn());
}

void AGamePlayerController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);

	NotifyLocalPawnChanged(InPawn);
}

void AGamePlayerController::ClientRefreshLocalPawn_Implementation(APawn* NewPawn)
{
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
		OnRespawnCountdownStarted(KillerName, RespawnDelay);
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
	}
}
