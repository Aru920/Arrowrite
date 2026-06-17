// Copyright Epic Games, Inc. All Rights Reserved.

#include "AbilitySystem/Player/PlayerAttributeSet.h"

#include "AbilitySystemComponent.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"
#include "Tags/GameplayTags.h"

UPlayerAttributeSet::UPlayerAttributeSet()
{
	InitMaxHealth(100.0f);
	InitHealth(100.0f);
	InitMovementSpeedMultiplier(1.0f);
	InitDamageTaken(0.0f);
}

void UPlayerAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, MovementSpeedMultiplier, COND_None, REPNOTIFY_Always);
}

void UPlayerAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetMaxHealthAttribute())
	{
		NewValue = FMath::Max(NewValue, 1.0f);
	}
	else if (Attribute == GetMovementSpeedMultiplierAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.1f, 2.0f);
	}
}

void UPlayerAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	const auto AddDeadTagIfHealthDepleted = [this]()
	{
		if (GetHealth() > 0.0f)
		{
			return;
		}

		if (UAbilitySystemComponent* OwningASC = GetOwningAbilitySystemComponent())
		{
			OwningASC->AddLooseGameplayTag(ArrowriteGameplayTags::State_Dead);
		}
	};

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));
		AddDeadTagIfHealthDepleted();
	}
	else if (Data.EvaluatedData.Attribute == GetMaxHealthAttribute())
	{
		SetMaxHealth(FMath::Max(GetMaxHealth(), 1.0f));
		SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));
	}
	else if (Data.EvaluatedData.Attribute == GetDamageTakenAttribute())
	{
		const float DamageDone = GetDamageTaken();
		SetDamageTaken(0.0f);

		if (DamageDone > 0.0f)
		{
			SetHealth(FMath::Clamp(GetHealth() - DamageDone, 0.0f, GetMaxHealth()));
			AddDeadTagIfHealthDepleted();
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

void UPlayerAttributeSet::OnRep_MovementSpeedMultiplier(const FGameplayAttributeData& OldMovementSpeedMultiplier) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, MovementSpeedMultiplier, OldMovementSpeedMultiplier);
}
