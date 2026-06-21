// Copyright Epic Games, Inc. All Rights Reserved.

#include "Player/GamePlayerState.h"

#include "AbilitySystem/Player/PlayerAbilitySystemComponent.h"
#include "AbilitySystem/Player/PlayerAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "Tags/GameplayTags.h"

AGamePlayerState::AGamePlayerState()
{
	SetNetUpdateFrequency(100.0f);

	AbilitySystemComponent = CreateDefaultSubobject<UPlayerAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<UPlayerAttributeSet>(TEXT("AttributeSet"));
}

void AGamePlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGamePlayerState, KillCount);
	DOREPLIFETIME(AGamePlayerState, DeathCount);
}

void AGamePlayerState::AddKill()
{
	SetKillCount(KillCount + 1);
}

void AGamePlayerState::AddDeath()
{
	SetDeathCount(DeathCount + 1);
}

void AGamePlayerState::ResetMatchScore()
{
	SetKillCount(0);
	SetDeathCount(0);
}

void AGamePlayerState::ResetCombatStateForRespawn()
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->CancelAbilities();

		FGameplayTagContainer TagsToClear;
		TagsToClear.AddTag(ArrowriteGameplayTags::State_Aiming);
		TagsToClear.AddTag(ArrowriteGameplayTags::State_DrawingBow);
		TagsToClear.AddTag(ArrowriteGameplayTags::State_Bow_Firing);
		TagsToClear.AddTag(ArrowriteGameplayTags::State_Bow_Reloading);
		TagsToClear.AddTag(ArrowriteGameplayTags::State_Dead);
		TagsToClear.AddTag(ArrowriteGameplayTags::State_HitReacting);
		TagsToClear.AddTag(ArrowriteGameplayTags::State_Burning);
		TagsToClear.AddTag(ArrowriteGameplayTags::State_Poisoned);
		TagsToClear.AddTag(ArrowriteGameplayTags::State_Frozen);
		TagsToClear.AddTag(ArrowriteGameplayTags::State_Stamina_RegenBlocked);

		for (const FGameplayTag& TagToClear : TagsToClear)
		{
			AbilitySystemComponent->SetLooseGameplayTagCount(TagToClear, 0);
		}

		AbilitySystemComponent->RemoveActiveEffectsWithGrantedTags(TagsToClear);
	}

	if (AbilitySystemComponent && AttributeSet)
	{
		AbilitySystemComponent->SetNumericAttributeBase(UPlayerAttributeSet::GetDamageTakenAttribute(), 0.0f);
		AbilitySystemComponent->SetNumericAttributeBase(UPlayerAttributeSet::GetHealthAttribute(), AttributeSet->GetMaxHealth());
		AbilitySystemComponent->SetNumericAttributeBase(UPlayerAttributeSet::GetStaminaAttribute(), AttributeSet->GetMaxStamina());
		AbilitySystemComponent->ForceReplication();
	}
}

void AGamePlayerState::OnRep_KillCount(int32 OldKillCount)
{
	OnKillCountChanged.Broadcast(KillCount, OldKillCount);
}

void AGamePlayerState::OnRep_DeathCount(int32 OldDeathCount)
{
	OnDeathCountChanged.Broadcast(DeathCount, OldDeathCount);
}

void AGamePlayerState::SetKillCount(int32 NewKillCount)
{
	NewKillCount = FMath::Max(0, NewKillCount);
	if (KillCount == NewKillCount)
	{
		return;
	}

	const int32 OldKillCount = KillCount;
	KillCount = NewKillCount;
	SetScore(static_cast<float>(KillCount));
	OnKillCountChanged.Broadcast(KillCount, OldKillCount);
}

void AGamePlayerState::SetDeathCount(int32 NewDeathCount)
{
	NewDeathCount = FMath::Max(0, NewDeathCount);
	if (DeathCount == NewDeathCount)
	{
		return;
	}

	const int32 OldDeathCount = DeathCount;
	DeathCount = NewDeathCount;
	OnDeathCountChanged.Broadcast(DeathCount, OldDeathCount);
}
