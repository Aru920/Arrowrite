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

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Deathmatch")
	void RecordPlayerDeath(AController* VictimController, AController* KillerController);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Deathmatch")
	void RequestPlayerRespawn(AController* Controller, float RespawnDelay = 3.0f);

protected:
	void RespawnPlayer(AController* Controller);
	void StartDeathmatch();
	void FinishDeathmatch();
	void HandleMatchTimerTick();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Deathmatch", meta = (ClampMin = "0"))
	int32 MatchDurationSeconds = 600;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Deathmatch")
	bool bAutoStartMatch = true;

private:
	TSet<TWeakObjectPtr<AController>> PendingRespawnControllers;
	FTimerHandle MatchTimerHandle;
};
