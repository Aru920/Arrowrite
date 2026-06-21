// Copyright Epic Games, Inc. All Rights Reserved.

#include "Player/GamePlayerState.h"

#include "AbilitySystem/Player/PlayerAbilitySystemComponent.h"
#include "AbilitySystem/Player/PlayerAttributeSet.h"
#include "Tags/GameplayTags.h"

AGamePlayerState::AGamePlayerState()
{
	SetNetUpdateFrequency(100.0f);

	AbilitySystemComponent = CreateDefaultSubobject<UPlayerAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<UPlayerAttributeSet>(TEXT("AttributeSet"));
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
	}
}
