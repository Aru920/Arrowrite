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

	for (const TSubclassOf<UGameplayEffect>& EffectClass : StartupGameplayEffects)
	{
		if (!EffectClass)
		{
			continue;
		}

		const UGameplayEffect* EffectCDO = EffectClass->GetDefaultObject<UGameplayEffect>();
		AbilitySystemComponent->ApplyGameplayEffectToSelf(
			EffectCDO,
			AbilityLevel,
			AbilitySystemComponent->MakeEffectContext()
		);
	}
}
