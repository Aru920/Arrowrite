// Copyright Epic Games, Inc. All Rights Reserved.

#include "AbilitySystem/Data/CharacterStartupData.h"

#include "AbilitySystem/BaseAbilitySystemComponent.h"
#include "GameplayEffect.h"

void UCharacterStartupData::GiveToAbilitySystemComponent(UBaseAbilitySystemComponent* AbilitySystemComponent, int32 AbilityLevel) const
{
	if (!AbilitySystemComponent)
	{
		return;
	}

	AbilitySystemComponent->GiveStartupAbilities(StartupAbilities, AbilityLevel);
	AbilitySystemComponent->ApplyStartupGameplayEffects(StartupGameplayEffects, AbilityLevel);
}
