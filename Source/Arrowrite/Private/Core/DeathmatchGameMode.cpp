// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/DeathmatchGameMode.h"

#include "Character/Player/PlayerCharacter.h"
#include "Core/DeathmatchGameState.h"
#include "Engine/World.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
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
		StartWarmup();
	}
}

void ADeathmatchGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	RefreshScoreboardState();
	GetWorldTimerManager().SetTimerForNextTick(this, &ThisClass::RefreshScoreboardState);

	if (ADeathmatchGameState* DeathmatchGameState = GetGameState<ADeathmatchGameState>();
		DeathmatchGameState && DeathmatchGameState->GetDeathmatchPhase() == EDeathmatchPhase::WaitingToStart)
	{
		SetMatchInputBlockedForController(NewPlayer, true);
		AController* NewController = NewPlayer;
		FTimerDelegate MatchInputBlockedDelegate;
		MatchInputBlockedDelegate.BindUObject(this, &ThisClass::SetMatchInputBlockedForController, NewController, true);
		GetWorldTimerManager().SetTimerForNextTick(MatchInputBlockedDelegate);
	}
}

void ADeathmatchGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	ClearPendingRespawn(Exiting, true);

	if (const UWorld* World = GetWorld(); !World || World->bIsTearingDown)
	{
		return;
	}

	RefreshScoreboardState();
	GetWorldTimerManager().SetTimerForNextTick(this, &ThisClass::RefreshScoreboardState);
}

void ADeathmatchGameMode::RecordPlayerDeath(AController* VictimController, AController* KillerController)
{
	if (!HasAuthority() || !IsValid(VictimController))
	{
		return;
	}

	ADeathmatchGameState* DeathmatchGameState = GetGameState<ADeathmatchGameState>();
	if (!DeathmatchGameState || DeathmatchGameState->GetDeathmatchPhase() != EDeathmatchPhase::InProgress)
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
	FString KillerName = TEXT("World");
	if (KillerController && KillerController != VictimController)
	{
		KillerPlayerState = KillerController->GetPlayerState<AGamePlayerState>();
		if (KillerPlayerState)
		{
			KillerPlayerState->AddKill();
			KillerName = KillerPlayerState->GetPlayerName();
		}
	}

	PendingRespawnKillerNames.Add(VictimController, KillerName);

	DeathmatchGameState->PushKillFeedEntry(KillerPlayerState, VictimPlayerState);
	DeathmatchGameState->NotifyScoreboardChanged();
}

void ADeathmatchGameMode::RequestPlayerRespawn(AController* Controller, float RespawnDelay)
{
	if (!HasAuthority() || !IsValid(Controller))
	{
		return;
	}

	RespawnDelay = FMath::Max(0.0f, RespawnDelay);

	const TWeakObjectPtr<AController> ControllerKey(Controller);
	if (PendingRespawnControllers.Contains(ControllerKey))
	{
		return;
	}

	PendingRespawnControllers.Add(ControllerKey);

	const FString KillerName = PendingRespawnKillerNames.FindRef(ControllerKey);
	if (AGamePlayerController* GamePlayerController = Cast<AGamePlayerController>(Controller))
	{
		GamePlayerController->ClientStartRespawnCountdown(KillerName.IsEmpty() ? TEXT("World") : KillerName, RespawnDelay);
	}

	if (RespawnDelay <= 0.0f)
	{
		RespawnPlayer(Controller);
		return;
	}

	FTimerHandle& RespawnTimerHandle = PendingRespawnTimerHandles.FindOrAdd(ControllerKey);
	FTimerDelegate RespawnDelegate;
	RespawnDelegate.BindUObject(this, &ThisClass::RespawnPlayer, Controller);
	GetWorldTimerManager().SetTimer(RespawnTimerHandle, RespawnDelegate, RespawnDelay, false);
}

void ADeathmatchGameMode::RespawnPlayer(AController* Controller)
{
	if (!HasAuthority() || !IsValid(Controller))
	{
		return;
	}

	ClearPendingRespawn(Controller, true);

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

	if (ADeathmatchGameState* DeathmatchGameState = GetGameState<ADeathmatchGameState>();
		DeathmatchGameState && DeathmatchGameState->GetDeathmatchPhase() == EDeathmatchPhase::WaitingToStart)
	{
		SetMatchInputBlockedForController(Controller, true);
	}

	if (AGamePlayerController* GamePlayerController = Cast<AGamePlayerController>(Controller))
	{
		GamePlayerController->ClientRefreshLocalPawn(Controller->GetPawn());
	}
}

void ADeathmatchGameMode::ClearPendingRespawn(AController* Controller, bool bClearTimer)
{
	if (!Controller)
	{
		return;
	}

	const TWeakObjectPtr<AController> ControllerKey(Controller);

	if (bClearTimer)
	{
		if (FTimerHandle* RespawnTimerHandle = PendingRespawnTimerHandles.Find(ControllerKey))
		{
			if (UWorld* World = GetWorld(); World && !World->bIsTearingDown)
			{
				World->GetTimerManager().ClearTimer(*RespawnTimerHandle);
			}
		}
	}

	PendingRespawnTimerHandles.Remove(ControllerKey);
	PendingRespawnControllers.Remove(ControllerKey);
	PendingRespawnKillerNames.Remove(ControllerKey);
}

void ADeathmatchGameMode::ClearAllPendingRespawns()
{
	if (UWorld* World = GetWorld(); World && !World->bIsTearingDown)
	{
		for (TPair<TWeakObjectPtr<AController>, FTimerHandle>& RespawnTimerPair : PendingRespawnTimerHandles)
		{
			World->GetTimerManager().ClearTimer(RespawnTimerPair.Value);
		}
	}

	PendingRespawnTimerHandles.Reset();
	PendingRespawnControllers.Reset();
	PendingRespawnKillerNames.Reset();
}

void ADeathmatchGameMode::StartWarmup()
{
	GetWorldTimerManager().ClearTimer(MatchTimerHandle);
	GetWorldTimerManager().ClearTimer(MatchRestartTimerHandle);
	ClearAllPendingRespawns();

	if (WarmupDurationSeconds <= 0)
	{
		StartDeathmatch();
		return;
	}

	if (ADeathmatchGameState* DeathmatchGameState = GetGameState<ADeathmatchGameState>())
	{
		DeathmatchGameState->SetMatchResult(nullptr, 0, false);
		DeathmatchGameState->SetDeathmatchPhase(EDeathmatchPhase::WaitingToStart);
		DeathmatchGameState->SetRemainingMatchTime(WarmupDurationSeconds);
	}

	SetMatchInputBlockedForAllPlayers(true);
	GetWorldTimerManager().SetTimer(WarmupTimerHandle, this, &ThisClass::HandleWarmupTimerTick, 1.0f, true);
}

void ADeathmatchGameMode::StartDeathmatch()
{
	GetWorldTimerManager().ClearTimer(WarmupTimerHandle);
	GetWorldTimerManager().ClearTimer(MatchRestartTimerHandle);
	ClearAllPendingRespawns();
	SetMatchInputBlockedForAllPlayers(false);

	if (ADeathmatchGameState* DeathmatchGameState = GetGameState<ADeathmatchGameState>())
	{
		DeathmatchGameState->SetMatchResult(nullptr, 0, false);
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
	GetWorldTimerManager().ClearTimer(WarmupTimerHandle);
	GetWorldTimerManager().ClearTimer(MatchTimerHandle);
	ClearAllPendingRespawns();
	SetMatchInputBlockedForAllPlayers(true);

	if (ADeathmatchGameState* DeathmatchGameState = GetGameState<ADeathmatchGameState>())
	{
		if (DeathmatchGameState->GetDeathmatchPhase() == EDeathmatchPhase::MatchEnded)
		{
			return;
		}

		APlayerState* WinnerPlayerState = nullptr;
		int32 WinningKills = 0;
		int32 PlayersWithWinningKills = 0;
		bool bHasPlayer = false;

		for (APlayerState* PlayerState : DeathmatchGameState->PlayerArray)
		{
			if (!PlayerState)
			{
				continue;
			}

			const AGamePlayerState* GamePlayerState = Cast<AGamePlayerState>(PlayerState);
			const int32 PlayerKills = GamePlayerState
				? GamePlayerState->GetKills()
				: FMath::Max(0, FMath::RoundToInt(PlayerState->GetScore()));

			if (!bHasPlayer || PlayerKills > WinningKills)
			{
				bHasPlayer = true;
				WinnerPlayerState = PlayerState;
				WinningKills = PlayerKills;
				PlayersWithWinningKills = 1;
			}
			else if (PlayerKills == WinningKills)
			{
				++PlayersWithWinningKills;
			}
		}

		const bool bWasTie = bHasPlayer && PlayersWithWinningKills > 1;
		DeathmatchGameState->SetMatchResult(bWasTie ? nullptr : WinnerPlayerState, WinningKills, bWasTie);
		DeathmatchGameState->SetRemainingMatchTime(0);
		DeathmatchGameState->SetDeathmatchPhase(EDeathmatchPhase::MatchEnded);
		ScheduleDeathmatchRestart();
	}
}

void ADeathmatchGameMode::ScheduleDeathmatchRestart()
{
	UWorld* World = GetWorld();
	if (!World || World->bIsTearingDown)
	{
		return;
	}

	World->GetTimerManager().ClearTimer(MatchRestartTimerHandle);

	if (MatchRestartDelaySeconds <= 0.0f)
	{
		RestartDeathmatchRound();
		return;
	}

	World->GetTimerManager().SetTimer(
		MatchRestartTimerHandle,
		this,
		&ThisClass::RestartDeathmatchRound,
		MatchRestartDelaySeconds,
		false);
}

void ADeathmatchGameMode::RestartDeathmatchRound()
{
	if (!HasAuthority())
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World || World->bIsTearingDown)
	{
		return;
	}

	World->GetTimerManager().ClearTimer(MatchRestartTimerHandle);
	ClearAllPendingRespawns();

	if (ADeathmatchGameState* DeathmatchGameState = GetGameState<ADeathmatchGameState>())
	{
		for (APlayerState* PlayerState : DeathmatchGameState->PlayerArray)
		{
			if (AGamePlayerState* GamePlayerState = Cast<AGamePlayerState>(PlayerState))
			{
				GamePlayerState->ResetMatchScore();
				GamePlayerState->ResetCombatStateForRespawn();
			}
		}

		DeathmatchGameState->NotifyScoreboardChanged();
	}

	StartWarmup();

	for (FConstPlayerControllerIterator PlayerControllerIt = World->GetPlayerControllerIterator(); PlayerControllerIt; ++PlayerControllerIt)
	{
		if (APlayerController* PlayerController = PlayerControllerIt->Get())
		{
			RespawnPlayer(PlayerController);
		}
	}

	RefreshScoreboardState();
}

void ADeathmatchGameMode::HandleWarmupTimerTick()
{
	ADeathmatchGameState* DeathmatchGameState = GetGameState<ADeathmatchGameState>();
	if (!DeathmatchGameState || DeathmatchGameState->GetDeathmatchPhase() != EDeathmatchPhase::WaitingToStart)
	{
		GetWorldTimerManager().ClearTimer(WarmupTimerHandle);
		return;
	}

	const int32 NewRemainingTime = DeathmatchGameState->GetRemainingMatchTime() - 1;
	DeathmatchGameState->SetRemainingMatchTime(NewRemainingTime);

	if (NewRemainingTime <= 0)
	{
		StartDeathmatch();
	}
}

void ADeathmatchGameMode::HandleMatchTimerTick()
{
	ADeathmatchGameState* DeathmatchGameState = GetGameState<ADeathmatchGameState>();
	if (!DeathmatchGameState || DeathmatchGameState->GetDeathmatchPhase() != EDeathmatchPhase::InProgress)
	{
		GetWorldTimerManager().ClearTimer(MatchTimerHandle);
		return;
	}

	const int32 NewRemainingTime = DeathmatchGameState->GetRemainingMatchTime() - 1;
	DeathmatchGameState->SetRemainingMatchTime(NewRemainingTime);

	if (NewRemainingTime <= 0)
	{
		FinishDeathmatch();
	}
}

void ADeathmatchGameMode::SetMatchInputBlockedForController(AController* Controller, bool bShouldBlock)
{
	if (!IsValid(Controller))
	{
		return;
	}

	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(Controller->GetPawn());
	if (!PlayerCharacter)
	{
		return;
	}

	PlayerCharacter->SetMatchInputBlocked(bShouldBlock);
}

void ADeathmatchGameMode::SetMatchInputBlockedForAllPlayers(bool bShouldBlock)
{
	UWorld* World = GetWorld();
	if (!World || World->bIsTearingDown)
	{
		return;
	}

	for (FConstPlayerControllerIterator PlayerControllerIt = World->GetPlayerControllerIterator(); PlayerControllerIt; ++PlayerControllerIt)
	{
		SetMatchInputBlockedForController(PlayerControllerIt->Get(), bShouldBlock);
	}
}

void ADeathmatchGameMode::RefreshScoreboardState()
{
	if (const UWorld* World = GetWorld(); !World || World->bIsTearingDown)
	{
		return;
	}

	if (ADeathmatchGameState* DeathmatchGameState = GetGameState<ADeathmatchGameState>())
	{
		DeathmatchGameState->NotifyScoreboardChanged();
	}
}
