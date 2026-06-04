// Copyright Epic Games, Inc. All Rights Reserved.

#include "AbilitySystem/BaseAbilitySystemComponent.h"

#include "AbilitySystem/Abilities/GameplayAbilityBase.h"

void UBaseAbilitySystemComponent::GiveStartupAbilities(const TArray<TSubclassOf<UGameplayAbilityBase>>& StartupAbilities, int32 AbilityLevel)
{
	if (!IsOwnerActorAuthoritative() || StartupAbilities.IsEmpty())
	{
		return;
	}

	for (const TSubclassOf<UGameplayAbilityBase>& AbilityClass : StartupAbilities)
	{
		if (!AbilityClass)
		{
			continue;
		}

		FGameplayAbilitySpec AbilitySpec(AbilityClass, AbilityLevel);
		AbilitySpec.SourceObject = GetAvatarActor();

		GiveAbility(AbilitySpec);
	}
}
