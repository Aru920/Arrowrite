// Copyright Epic Games, Inc. All Rights Reserved.

#include "Player/GamePlayerController.h"

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

void AGamePlayerController::NotifyLocalPawnChanged(APawn* NewPawn)
{
	if (IsLocalController())
	{
		OnLocalPawnChanged(NewPawn);
	}
}
