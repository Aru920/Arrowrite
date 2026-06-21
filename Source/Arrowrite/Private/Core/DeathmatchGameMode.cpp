// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/DeathmatchGameMode.h"

#include "Character/Player/PlayerCharacter.h"
#include "Core/DeathmatchGameState.h"
#include "Engine/World.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"
#include "TimerManager.h"
#include "Player/GamePlayerController.h"
#include "Player/GamePlayerState.h"
#include "Player/PlayerEquipmentComponent.h"

ADeathmatchGameMode::ADeathmatchGameMode()
{
	DefaultPawnClass = APlayerCharacter::StaticClass();
	PlayerControllerClass = AGamePlayerController::StaticClass();
	PlayerStateClass = AGamePlayerState::StaticClass();
	GameStateClass = ADeathmatchGameState::StaticClass();
}

void ADeathmatchGameMode::RequestPlayerRespawn(AController* Controller, float RespawnDelay)
{
	if (!HasAuthority() || !Controller)
	{
		return;
	}

	const TWeakObjectPtr<AController> ControllerKey(Controller);
	if (PendingRespawnControllers.Contains(ControllerKey))
	{
		return;
	}

	PendingRespawnControllers.Add(ControllerKey);

	if (RespawnDelay <= 0.0f)
	{
		RespawnPlayer(Controller);
		return;
	}

	FTimerHandle RespawnTimerHandle;
	FTimerDelegate RespawnDelegate;
	RespawnDelegate.BindUObject(this, &ThisClass::RespawnPlayer, Controller);
	GetWorldTimerManager().SetTimer(RespawnTimerHandle, RespawnDelegate, RespawnDelay, false);
}

void ADeathmatchGameMode::RespawnPlayer(AController* Controller)
{
	if (!HasAuthority() || !Controller)
	{
		return;
	}

	PendingRespawnControllers.Remove(TWeakObjectPtr<AController>(Controller));

	if (AGamePlayerState* GamePlayerState = Controller->GetPlayerState<AGamePlayerState>())
	{
		GamePlayerState->ResetCombatStateForRespawn();
	}

	if (APawn* OldPawn = Controller->GetPawn())
	{
		if (APlayerCharacter* OldPlayerCharacter = Cast<APlayerCharacter>(OldPawn))
		{
			if (UPlayerEquipmentComponent* EquipmentComponent = OldPlayerCharacter->GetEquipmentComponent())
			{
				EquipmentComponent->DestroyCarriedWeapons();
			}
		}

		Controller->UnPossess();
		OldPawn->Destroy();
	}

	RestartPlayer(Controller);
}
