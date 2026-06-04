// Copyright Epic Games, Inc. All Rights Reserved.

#include "Player/GamePlayerState.h"

#include "AbilitySystem/Player/PlayerAbilitySystemComponent.h"
#include "AbilitySystem/Player/PlayerAttributeSet.h"

AGamePlayerState::AGamePlayerState()
{
	SetNetUpdateFrequency(100.0f);

	AbilitySystemComponent = CreateDefaultSubobject<UPlayerAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<UPlayerAttributeSet>(TEXT("AttributeSet"));
}
