// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/DeathmatchGameState.h"

#include "Net/UnrealNetwork.h"

void ADeathmatchGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADeathmatchGameState, DeathmatchPhase);
	DOREPLIFETIME(ADeathmatchGameState, RemainingMatchTime);
	DOREPLIFETIME(ADeathmatchGameState, LatestKillFeedEntry);
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
