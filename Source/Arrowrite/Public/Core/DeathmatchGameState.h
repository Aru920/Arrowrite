// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "DeathmatchGameState.generated.h"

class APlayerState;

UENUM(BlueprintType)
enum class EDeathmatchPhase : uint8
{
	WaitingToStart,
	InProgress,
	MatchEnded
};

USTRUCT(BlueprintType)
struct FDeathmatchKillFeedEntry
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Deathmatch")
	TObjectPtr<APlayerState> KillerPlayerState = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Deathmatch")
	TObjectPtr<APlayerState> VictimPlayerState = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Deathmatch")
	int32 EntryId = 0;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDeathmatchPhaseChangedDelegate, EDeathmatchPhase, NewPhase, EDeathmatchPhase, OldPhase);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMatchTimeChangedDelegate, int32, NewRemainingTime, int32, OldRemainingTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDeathmatchKillFeedDelegate, FDeathmatchKillFeedEntry, KillFeedEntry);

UCLASS()
class ARROWRITE_API ADeathmatchGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintPure, Category = "Deathmatch")
	EDeathmatchPhase GetDeathmatchPhase() const { return DeathmatchPhase; }

	UFUNCTION(BlueprintPure, Category = "Deathmatch")
	int32 GetRemainingMatchTime() const { return RemainingMatchTime; }

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Deathmatch")
	void SetDeathmatchPhase(EDeathmatchPhase NewPhase);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Deathmatch")
	void SetRemainingMatchTime(int32 NewRemainingTime);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Deathmatch")
	void PushKillFeedEntry(APlayerState* KillerPlayerState, APlayerState* VictimPlayerState);

	UPROPERTY(BlueprintAssignable, Category = "Deathmatch")
	FDeathmatchPhaseChangedDelegate OnDeathmatchPhaseChanged;

	UPROPERTY(BlueprintAssignable, Category = "Deathmatch")
	FMatchTimeChangedDelegate OnRemainingMatchTimeChanged;

	UPROPERTY(BlueprintAssignable, Category = "Deathmatch")
	FDeathmatchKillFeedDelegate OnKillFeedEntryAdded;

protected:
	UFUNCTION()
	void OnRep_DeathmatchPhase(EDeathmatchPhase OldPhase);

	UFUNCTION()
	void OnRep_RemainingMatchTime(int32 OldRemainingTime);

	UFUNCTION()
	void OnRep_LatestKillFeedEntry();

	UPROPERTY(ReplicatedUsing = OnRep_DeathmatchPhase, BlueprintReadOnly, Category = "Deathmatch")
	EDeathmatchPhase DeathmatchPhase = EDeathmatchPhase::WaitingToStart;

	UPROPERTY(ReplicatedUsing = OnRep_RemainingMatchTime, BlueprintReadOnly, Category = "Deathmatch")
	int32 RemainingMatchTime = 0;

	UPROPERTY(ReplicatedUsing = OnRep_LatestKillFeedEntry, BlueprintReadOnly, Category = "Deathmatch")
	FDeathmatchKillFeedEntry LatestKillFeedEntry;

	int32 NextKillFeedEntryId = 1;
};
