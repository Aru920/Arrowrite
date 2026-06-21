// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/DeathmatchGameMode.h"

#include "Character/Player/PlayerCharacter.h"
#include "Core/DeathmatchGameState.h"
#include "Engine/World.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerState.h"
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

void ADeathmatchGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (bAutoStartMatch)
	{
		StartDeathmatch();
	}
}

void ADeathmatchGameMode::RecordPlayerDeath(AController* VictimController, AController* KillerController)
{
	if (!HasAuthority() || !VictimController)
	{
		return;
	}

	AGamePlayerState* VictimPlayerState = VictimController->GetPlayerState<AGamePlayerState>();
	if (!VictimPlayerState)
	{
		return;
	}

	VictimPlayerState->AddDeath();

	AGamePlayerState* KillerPlayerState = nullptr;
	if (KillerController && KillerController != VictimController)
	{
		KillerPlayerState = KillerController->GetPlayerState<AGamePlayerState>();
		if (KillerPlayerState)
		{
			KillerPlayerState->AddKill();
		}
	}

	if (ADeathmatchGameState* DeathmatchGameState = GetGameState<ADeathmatchGameState>())
	{
		DeathmatchGameState->PushKillFeedEntry(KillerPlayerState, VictimPlayerState);
	}
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

	if (AGamePlayerController* GamePlayerController = Cast<AGamePlayerController>(Controller))
	{
		GamePlayerController->ClientRefreshLocalPawn(Controller->GetPawn());
	}
}

void ADeathmatchGameMode::StartDeathmatch()
{
	if (ADeathmatchGameState* DeathmatchGameState = GetGameState<ADeathmatchGameState>())
	{
		DeathmatchGameState->SetDeathmatchPhase(EDeathmatchPhase::InProgress);
		DeathmatchGameState->SetRemainingMatchTime(MatchDurationSeconds);
	}

	if (MatchDurationSeconds > 0)
	{
		GetWorldTimerManager().SetTimer(MatchTimerHandle, this, &ThisClass::HandleMatchTimerTick, 1.0f, true);
	}
}

void ADeathmatchGameMode::FinishDeathmatch()
{
	GetWorldTimerManager().ClearTimer(MatchTimerHandle);

	if (ADeathmatchGameState* DeathmatchGameState = GetGameState<ADeathmatchGameState>())
	{
		DeathmatchGameState->SetRemainingMatchTime(0);
		DeathmatchGameState->SetDeathmatchPhase(EDeathmatchPhase::MatchEnded);
	}
}

void ADeathmatchGameMode::HandleMatchTimerTick()
{
	ADeathmatchGameState* DeathmatchGameState = GetGameState<ADeathmatchGameState>();
	if (!DeathmatchGameState)
	{
		return;
	}

	const int32 NewRemainingTime = DeathmatchGameState->GetRemainingMatchTime() - 1;
	DeathmatchGameState->SetRemainingMatchTime(NewRemainingTime);

	if (NewRemainingTime <= 0)
	{
		FinishDeathmatch();
	}
}
