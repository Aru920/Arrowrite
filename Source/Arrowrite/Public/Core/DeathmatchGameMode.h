// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "DeathmatchGameMode.generated.h"

class AController;

UCLASS()
class ARROWRITE_API ADeathmatchGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ADeathmatchGameMode();

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Deathmatch")
	void RequestPlayerRespawn(AController* Controller, float RespawnDelay = 3.0f);

protected:
	void RespawnPlayer(AController* Controller);

private:
	TSet<TWeakObjectPtr<AController>> PendingRespawnControllers;
};
