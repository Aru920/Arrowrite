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

USTRUCT(BlueprintType)
struct FDeathmatchScoreboardEntry
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Deathmatch")
	TObjectPtr<APlayerState> PlayerState = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Deathmatch")
	int32 Kills = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Deathmatch")
	int32 Deaths = 0;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDeathmatchPhaseChangedDelegate, EDeathmatchPhase, NewPhase, EDeathmatchPhase, OldPhase);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMatchTimeChangedDelegate, int32, NewRemainingTime, int32, OldRemainingTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDeathmatchKillFeedDelegate, FDeathmatchKillFeedEntry, KillFeedEntry);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDeathmatchScoreboardChangedDelegate);

UCLASS()
class ARROWRITE_API ADeathmatchGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	ADeathmatchGameState();

	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void AddPlayerState(APlayerState* PlayerState) override;
	virtual void RemovePlayerState(APlayerState* PlayerState) override;

	UFUNCTION(BlueprintPure, Category = "Deathmatch")
	EDeathmatchPhase GetDeathmatchPhase() const { return DeathmatchPhase; }

	UFUNCTION(BlueprintPure, Category = "Deathmatch")
	int32 GetRemainingMatchTime() const { return RemainingMatchTime; }

	UFUNCTION(BlueprintPure, Category = "Deathmatch")
	const TArray<FDeathmatchScoreboardEntry>& GetScoreboardEntries() const { return ScoreboardEntries; }

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Deathmatch")
	void SetDeathmatchPhase(EDeathmatchPhase NewPhase);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Deathmatch")
	void SetRemainingMatchTime(int32 NewRemainingTime);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Deathmatch")
	void PushKillFeedEntry(APlayerState* KillerPlayerState, APlayerState* VictimPlayerState);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Deathmatch")
	void NotifyScoreboardChanged();

	UPROPERTY(BlueprintAssignable, Category = "Deathmatch")
	FDeathmatchPhaseChangedDelegate OnDeathmatchPhaseChanged;

	UPROPERTY(BlueprintAssignable, Category = "Deathmatch")
	FMatchTimeChangedDelegate OnRemainingMatchTimeChanged;

	UPROPERTY(BlueprintAssignable, Category = "Deathmatch")
	FDeathmatchKillFeedDelegate OnKillFeedEntryAdded;

	UPROPERTY(BlueprintAssignable, Category = "Deathmatch")
	FDeathmatchScoreboardChangedDelegate OnScoreboardChanged;

protected:
	UFUNCTION()
	void OnRep_DeathmatchPhase(EDeathmatchPhase OldPhase);

	UFUNCTION()
	void OnRep_RemainingMatchTime(int32 OldRemainingTime);

	UFUNCTION()
	void OnRep_LatestKillFeedEntry();

	UFUNCTION()
	void OnRep_ScoreboardEntries();

	void RebuildScoreboardEntries();

	UPROPERTY(ReplicatedUsing = OnRep_DeathmatchPhase, BlueprintReadOnly, Category = "Deathmatch")
	EDeathmatchPhase DeathmatchPhase = EDeathmatchPhase::WaitingToStart;

	UPROPERTY(ReplicatedUsing = OnRep_RemainingMatchTime, BlueprintReadOnly, Category = "Deathmatch")
	int32 RemainingMatchTime = 0;

	UPROPERTY(ReplicatedUsing = OnRep_LatestKillFeedEntry, BlueprintReadOnly, Category = "Deathmatch")
	FDeathmatchKillFeedEntry LatestKillFeedEntry;

	UPROPERTY(ReplicatedUsing = OnRep_ScoreboardEntries, BlueprintReadOnly, Category = "Deathmatch")
	TArray<FDeathmatchScoreboardEntry> ScoreboardEntries;

	int32 NextKillFeedEntryId = 1;
};
