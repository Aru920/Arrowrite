// Copyright Epic Games, Inc. All Rights Reserved.

#include "AbilitySystem/BaseAbilitySystemComponent.h"

#include "AbilitySystem/Abilities/GameplayAbilityBase.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayEffect.h"

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

		if (FGameplayAbilitySpec* ExistingAbilitySpec = FindAbilitySpecOfClass(AbilityClass))
		{
			RefreshExistingStartupAbility(*ExistingAbilitySpec);
			continue;
		}

		FGameplayAbilitySpec AbilitySpec(AbilityClass, AbilityLevel);
		AbilitySpec.SourceObject = GetAvatarActor();

		if (const UGameplayAbilityBase* AbilityCDO = AbilityClass->GetDefaultObject<UGameplayAbilityBase>())
		{
			if (AbilityCDO->GetInputTag().IsValid())
			{
				AbilitySpec.GetDynamicSpecSourceTags().AddTag(AbilityCDO->GetInputTag());
			}
		}

		GiveAbility(AbilitySpec);
	}
}

void UBaseAbilitySystemComponent::ApplyStartupGameplayEffects(const TArray<TSubclassOf<UGameplayEffect>>& StartupGameplayEffects, int32 AbilityLevel)
{
	if (!IsOwnerActorAuthoritative() || bStartupGameplayEffectsApplied || StartupGameplayEffects.IsEmpty())
	{
		return;
	}

	for (const TSubclassOf<UGameplayEffect>& EffectClass : StartupGameplayEffects)
	{
		if (!EffectClass)
		{
			continue;
		}

		const UGameplayEffect* EffectCDO = EffectClass->GetDefaultObject<UGameplayEffect>();
		ApplyGameplayEffectToSelf(
			EffectCDO,
			AbilityLevel,
			MakeEffectContext()
		);
	}

	bStartupGameplayEffectsApplied = true;
}

void UBaseAbilitySystemComponent::GiveWeaponAbilities(const FWeaponAbilitySet& AbilitySet, int32 AbilityLevel)
{
	GiveStartupAbilities(AbilitySet.GrantedAbilities, AbilityLevel);
}

void UBaseAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())
	{
		return;
	}

	ABILITYLIST_SCOPE_LOCK();
	for (FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
	{
		if (!AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		{
			continue;
		}

		AbilitySpecInputPressed(AbilitySpec);

		if (AbilitySpec.IsActive())
		{
			continue;
		}

		const UGameplayAbilityBase* AbilityCDO = Cast<UGameplayAbilityBase>(AbilitySpec.Ability);
		if (AbilityCDO && (AbilityCDO->GetActivationPolicy() == EAbilityActivationPolicy::OnInputTriggered || AbilityCDO->GetActivationPolicy() == EAbilityActivationPolicy::OnInputHeld))
		{
			TryActivateAbility(AbilitySpec.Handle);
		}
	}
}

void UBaseAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())
	{
		return;
	}

	ABILITYLIST_SCOPE_LOCK();
	for (FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
	{
		if (!AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		{
			continue;
		}

		AbilitySpecInputReleased(AbilitySpec);

		if (!AbilitySpec.IsActive())
		{
			continue;
		}

		for (UGameplayAbility* AbilityInstance : AbilitySpec.GetAbilityInstances())
		{
			if (!AbilityInstance)
			{
				continue;
			}

			InvokeReplicatedEvent(
				EAbilityGenericReplicatedEvent::InputReleased,
				AbilitySpec.Handle,
				AbilityInstance->GetCurrentActivationInfo().GetActivationPredictionKey());
		}
	}
}

FGameplayAbilitySpec* UBaseAbilitySystemComponent::FindAbilitySpecOfClass(TSubclassOf<UGameplayAbilityBase> AbilityClass)
{
	if (!AbilityClass)
	{
		return nullptr;
	}

	for (FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
	{
		if (AbilitySpec.Ability && AbilitySpec.Ability->GetClass() == AbilityClass)
		{
			return &AbilitySpec;
		}
	}

	return nullptr;
}

void UBaseAbilitySystemComponent::RefreshExistingStartupAbility(FGameplayAbilitySpec& AbilitySpec)
{
	AbilitySpec.SourceObject = GetAvatarActor();

	const UGameplayAbilityBase* AbilityCDO = Cast<UGameplayAbilityBase>(AbilitySpec.Ability);
	if (AbilityCDO && AbilityCDO->GetActivationPolicy() == EAbilityActivationPolicy::OnGiven && !AbilitySpec.IsActive())
	{
		TryActivateAbility(AbilitySpec.Handle);
	}
}
