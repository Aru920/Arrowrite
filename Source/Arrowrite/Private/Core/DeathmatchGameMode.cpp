// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/DeathmatchGameMode.h"

#include "Character/Player/PlayerCharacter.h"
#include "Core/DeathmatchGameState.h"
#include "Player/GamePlayerController.h"
#include "Player/GamePlayerState.h"

ADeathmatchGameMode::ADeathmatchGameMode()
{
	DefaultPawnClass = APlayerCharacter::StaticClass();
	PlayerControllerClass = AGamePlayerController::StaticClass();
	PlayerStateClass = AGamePlayerState::StaticClass();
	GameStateClass = ADeathmatchGameState::StaticClass();
}
