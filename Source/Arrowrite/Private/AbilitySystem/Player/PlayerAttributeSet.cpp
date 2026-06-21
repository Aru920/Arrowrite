// Copyright Epic Games, Inc. All Rights Reserved.

#include "AbilitySystem/Player/PlayerAttributeSet.h"

#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"
#include "Tags/GameplayTags.h"

UPlayerAttributeSet::UPlayerAttributeSet()
{
	InitMaxHealth(100.0f);
	InitHealth(100.0f);
	InitMaxStamina(100.0f);
	InitStamina(100.0f);
	InitMovementSpeedMultiplier(1.0f);
	InitDamageTaken(0.0f);
}

void UPlayerAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, Stamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, MaxStamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, MovementSpeedMultiplier, COND_None, REPNOTIFY_Always);
}

void UPlayerAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetMaxHealthAttribute())
	{
		NewValue = FMath::Max(NewValue, 1.0f);
	}
	else if (Attribute == GetMaxStaminaAttribute())
	{
		NewValue = FMath::Max(NewValue, 1.0f);
	}
	else if (Attribute == GetStaminaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxStamina());
	}
	else if (Attribute == GetMovementSpeedMultiplierAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.1f, 2.0f);
	}
}

void UPlayerAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	const auto HandleDeathIfHealthDepleted = [this, &Data]()
	{
		if (GetHealth() > 0.0f)
		{
			return;
		}

		if (UAbilitySystemComponent* OwningASC = GetOwningAbilitySystemComponent())
		{
			if (OwningASC->HasMatchingGameplayTag(ArrowriteGameplayTags::State_Dead))
			{
				return;
			}

			OwningASC->AddLooseGameplayTag(ArrowriteGameplayTags::State_Dead);

			FGameplayEventData EventData;
			EventData.EventTag = ArrowriteGameplayTags::Event_Player_Death;
			EventData.Instigator = Data.EffectSpec.GetContext().GetOriginalInstigator();
			EventData.Target = OwningASC->GetAvatarActor();
			EventData.ContextHandle = Data.EffectSpec.GetContext();

			OwningASC->HandleGameplayEvent(ArrowriteGameplayTags::Event_Player_Death, &EventData);
		}
	};

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));
		HandleDeathIfHealthDepleted();
	}
	else if (Data.EvaluatedData.Attribute == GetMaxHealthAttribute())
	{
		SetMaxHealth(FMath::Max(GetMaxHealth(), 1.0f));
		SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));
	}
	else if (Data.EvaluatedData.Attribute == GetStaminaAttribute())
	{
		SetStamina(FMath::Clamp(GetStamina(), 0.0f, GetMaxStamina()));
	}
	else if (Data.EvaluatedData.Attribute == GetMaxStaminaAttribute())
	{
		SetMaxStamina(FMath::Max(GetMaxStamina(), 1.0f));
		SetStamina(FMath::Clamp(GetStamina(), 0.0f, GetMaxStamina()));
	}
	else if (Data.EvaluatedData.Attribute == GetDamageTakenAttribute())
	{
		const float DamageDone = GetDamageTaken();
		SetDamageTaken(0.0f);

		if (DamageDone > 0.0f)
		{
			SetHealth(FMath::Clamp(GetHealth() - DamageDone, 0.0f, GetMaxHealth()));
			HandleDeathIfHealthDepleted();
		}
	}
	else if (Data.EvaluatedData.Attribute == GetMovementSpeedMultiplierAttribute())
	{
		SetMovementSpeedMultiplier(FMath::Clamp(GetMovementSpeedMultiplier(), 0.1f, 2.0f));
	}
}

void UPlayerAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, Health, OldHealth);
}

void UPlayerAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, MaxHealth, OldMaxHealth);
}

void UPlayerAttributeSet::OnRep_Stamina(const FGameplayAttributeData& OldStamina) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, Stamina, OldStamina);
}

void UPlayerAttributeSet::OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, MaxStamina, OldMaxStamina);
}

void UPlayerAttributeSet::OnRep_MovementSpeedMultiplier(const FGameplayAttributeData& OldMovementSpeedMultiplier) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, MovementSpeedMultiplier, OldMovementSpeedMultiplier);
}
