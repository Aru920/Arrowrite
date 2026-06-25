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

USTRUCT(BlueprintType)
struct FDeathmatchMatchResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Deathmatch")
	TObjectPtr<APlayerState> WinnerPlayerState = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Deathmatch")
	int32 WinningKills = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Deathmatch")
	bool bWasTie = false;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDeathmatchPhaseChangedDelegate, EDeathmatchPhase, NewPhase, EDeathmatchPhase, OldPhase);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMatchTimeChangedDelegate, int32, NewRemainingTime, int32, OldRemainingTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDeathmatchKillFeedDelegate, FDeathmatchKillFeedEntry, KillFeedEntry);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDeathmatchScoreboardChangedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDeathmatchMatchResultChangedDelegate, FDeathmatchMatchResult, MatchResult);

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

	UFUNCTION(BlueprintPure, Category = "Deathmatch|Result")
	const FDeathmatchMatchResult& GetMatchResult() const { return MatchResult; }

	UFUNCTION(BlueprintPure, Category = "Deathmatch|Result")
	APlayerState* GetWinnerPlayerState() const { return MatchResult.WinnerPlayerState.Get(); }

	UFUNCTION(BlueprintPure, Category = "Deathmatch|Result")
	int32 GetWinningKills() const { return MatchResult.WinningKills; }

	UFUNCTION(BlueprintPure, Category = "Deathmatch|Result")
	bool WasMatchTie() const { return MatchResult.bWasTie; }

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Deathmatch")
	void SetDeathmatchPhase(EDeathmatchPhase NewPhase);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Deathmatch")
	void SetRemainingMatchTime(int32 NewRemainingTime);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Deathmatch|Result")
	void SetMatchResult(APlayerState* WinnerPlayerState, int32 InWinningKills, bool bInWasTie);

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

	UPROPERTY(BlueprintAssignable, Category = "Deathmatch|Result")
	FDeathmatchMatchResultChangedDelegate OnMatchResultChanged;

protected:
	UFUNCTION()
	void OnRep_DeathmatchPhase(EDeathmatchPhase OldPhase);

	UFUNCTION()
	void OnRep_RemainingMatchTime(int32 OldRemainingTime);

	UFUNCTION()
	void OnRep_LatestKillFeedEntry();

	UFUNCTION()
	void OnRep_ScoreboardEntries();

	UFUNCTION()
	void OnRep_MatchResult();

	void RebuildScoreboardEntries();

	UPROPERTY(ReplicatedUsing = OnRep_DeathmatchPhase, BlueprintReadOnly, Category = "Deathmatch")
	EDeathmatchPhase DeathmatchPhase = EDeathmatchPhase::WaitingToStart;

	UPROPERTY(ReplicatedUsing = OnRep_RemainingMatchTime, BlueprintReadOnly, Category = "Deathmatch")
	int32 RemainingMatchTime = 0;

	UPROPERTY(ReplicatedUsing = OnRep_LatestKillFeedEntry, BlueprintReadOnly, Category = "Deathmatch")
	FDeathmatchKillFeedEntry LatestKillFeedEntry;

	UPROPERTY(ReplicatedUsing = OnRep_ScoreboardEntries, BlueprintReadOnly, Category = "Deathmatch")
	TArray<FDeathmatchScoreboardEntry> ScoreboardEntries;

	UPROPERTY(ReplicatedUsing = OnRep_MatchResult, BlueprintReadOnly, Category = "Deathmatch|Result")
	FDeathmatchMatchResult MatchResult;

	int32 NextKillFeedEntryId = 1;
};
