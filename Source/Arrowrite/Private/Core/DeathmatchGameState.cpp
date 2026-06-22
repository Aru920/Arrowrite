// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/DeathmatchGameState.h"

#include "Player/GamePlayerState.h"
#include "Net/UnrealNetwork.h"

ADeathmatchGameState::ADeathmatchGameState()
{
	SetNetUpdateFrequency(10.0f);
}

void ADeathmatchGameState::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		RebuildScoreboardEntries();
	}
}

void ADeathmatchGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADeathmatchGameState, DeathmatchPhase);
	DOREPLIFETIME(ADeathmatchGameState, RemainingMatchTime);
	DOREPLIFETIME(ADeathmatchGameState, LatestKillFeedEntry);
	DOREPLIFETIME(ADeathmatchGameState, ScoreboardEntries);
}

void ADeathmatchGameState::AddPlayerState(APlayerState* PlayerState)
{
	Super::AddPlayerState(PlayerState);

	RebuildScoreboardEntries();
}

void ADeathmatchGameState::RemovePlayerState(APlayerState* PlayerState)
{
	Super::RemovePlayerState(PlayerState);

	RebuildScoreboardEntries();
}

void ADeathmatchGameState::SetDeathmatchPhase(EDeathmatchPhase NewPhase)
{
	if (DeathmatchPhase == NewPhase)
	{
		return;
	}

	const EDeathmatchPhase OldPhase = DeathmatchPhase;
	DeathmatchPhase = NewPhase;
	OnDeathmatchPhaseChanged.Broadcast(DeathmatchPhase, OldPhase);
	ForceNetUpdate();
}

void ADeathmatchGameState::SetRemainingMatchTime(int32 NewRemainingTime)
{
	NewRemainingTime = FMath::Max(0, NewRemainingTime);
	if (RemainingMatchTime == NewRemainingTime)
	{
		return;
	}

	const int32 OldRemainingTime = RemainingMatchTime;
	RemainingMatchTime = NewRemainingTime;
	OnRemainingMatchTimeChanged.Broadcast(RemainingMatchTime, OldRemainingTime);
	ForceNetUpdate();
}

void ADeathmatchGameState::PushKillFeedEntry(APlayerState* KillerPlayerState, APlayerState* VictimPlayerState)
{
	if (!VictimPlayerState)
	{
		return;
	}

	LatestKillFeedEntry.KillerPlayerState = KillerPlayerState;
	LatestKillFeedEntry.VictimPlayerState = VictimPlayerState;
	LatestKillFeedEntry.EntryId = NextKillFeedEntryId++;

	OnKillFeedEntryAdded.Broadcast(LatestKillFeedEntry);
	ForceNetUpdate();
}

void ADeathmatchGameState::NotifyScoreboardChanged()
{
	RebuildScoreboardEntries();
}

void ADeathmatchGameState::OnRep_DeathmatchPhase(EDeathmatchPhase OldPhase)
{
	OnDeathmatchPhaseChanged.Broadcast(DeathmatchPhase, OldPhase);
}

void ADeathmatchGameState::OnRep_RemainingMatchTime(int32 OldRemainingTime)
{
	OnRemainingMatchTimeChanged.Broadcast(RemainingMatchTime, OldRemainingTime);
}

void ADeathmatchGameState::OnRep_LatestKillFeedEntry()
{
	if (LatestKillFeedEntry.EntryId > 0)
	{
		OnKillFeedEntryAdded.Broadcast(LatestKillFeedEntry);
	}
}

void ADeathmatchGameState::OnRep_ScoreboardEntries()
{
	OnScoreboardChanged.Broadcast();
}

void ADeathmatchGameState::RebuildScoreboardEntries()
{
	ScoreboardEntries.Reset();

	for (APlayerState* PlayerState : PlayerArray)
	{
		if (!PlayerState)
		{
			continue;
		}

		FDeathmatchScoreboardEntry Entry;
		Entry.PlayerState = PlayerState;

		if (const AGamePlayerState* GamePlayerState = Cast<AGamePlayerState>(PlayerState))
		{
			Entry.Kills = GamePlayerState->GetKills();
			Entry.Deaths = GamePlayerState->GetDeaths();
		}

		ScoreboardEntries.Add(Entry);
	}

	OnScoreboardChanged.Broadcast();
	ForceNetUpdate();
}
