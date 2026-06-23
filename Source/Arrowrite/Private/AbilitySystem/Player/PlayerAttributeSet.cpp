// Copyright Epic Games, Inc. All Rights Reserved.

#include "AbilitySystem/Player/PlayerAttributeSet.h"

#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "Character/Player/PlayerCharacter.h"
#include "Core/DeathmatchGameMode.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"
#include "Tags/GameplayTags.h"

namespace
{
	FGameplayTag ResolveDamagePresentationTag(const UAbilitySystemComponent* TargetAbilitySystemComponent)
	{
		if (!TargetAbilitySystemComponent)
		{
			return FGameplayTag();
		}

		if (TargetAbilitySystemComponent->HasMatchingGameplayTag(ArrowriteGameplayTags::State_Burning))
		{
			return ArrowriteGameplayTags::State_Burning;
		}

		if (TargetAbilitySystemComponent->HasMatchingGameplayTag(ArrowriteGameplayTags::State_Poisoned))
		{
			return ArrowriteGameplayTags::State_Poisoned;
		}

		if (TargetAbilitySystemComponent->HasMatchingGameplayTag(ArrowriteGameplayTags::State_Frozen))
		{
			return ArrowriteGameplayTags::State_Frozen;
		}

		return FGameplayTag();
	}
}

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

			AActor* TargetActor = OwningASC->GetAvatarActor();
			APawn* VictimPawn = Cast<APawn>(TargetActor);
			AController* VictimController = VictimPawn ? VictimPawn->GetController() : nullptr;

			AController* KillerController = nullptr;
			AActor* InstigatorActor = Data.EffectSpec.GetContext().GetOriginalInstigator();
			if (APawn* InstigatorPawn = Cast<APawn>(InstigatorActor))
			{
				KillerController = InstigatorPawn->GetController();
			}
			else if (AController* InstigatorController = Cast<AController>(InstigatorActor))
			{
				KillerController = InstigatorController;
			}
			else if (InstigatorActor)
			{
				KillerController = InstigatorActor->GetInstigatorController();
			}

			if (ADeathmatchGameMode* DeathmatchGameMode = TargetActor && TargetActor->GetWorld()
				? TargetActor->GetWorld()->GetAuthGameMode<ADeathmatchGameMode>()
				: nullptr)
			{
				DeathmatchGameMode->RecordPlayerDeath(VictimController, KillerController);
			}

			FGameplayEventData EventData;
			EventData.EventTag = ArrowriteGameplayTags::Event_Player_Death;
			EventData.Instigator = Data.EffectSpec.GetContext().GetOriginalInstigator();
			EventData.Target = OwningASC->GetAvatarActor();
			EventData.ContextHandle = Data.EffectSpec.GetContext();

			OwningASC->HandleGameplayEvent(ArrowriteGameplayTags::Event_Player_Death, &EventData);
			OwningASC->SetLooseGameplayTagCount(ArrowriteGameplayTags::State_Dead, 1);
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
			UAbilitySystemComponent* OwningASC = GetOwningAbilitySystemComponent();
			AActor* TargetActor = OwningASC ? OwningASC->GetAvatarActor() : nullptr;
			AActor* DamageSource = Data.EffectSpec.GetContext().GetOriginalInstigator();

			SetHealth(FMath::Clamp(GetHealth() - DamageDone, 0.0f, GetMaxHealth()));

			if (APlayerCharacter* TargetPlayerCharacter = Cast<APlayerCharacter>(TargetActor))
			{
				TargetPlayerCharacter->ShowDamageNumber(DamageDone, ResolveDamagePresentationTag(OwningASC), DamageSource);
			}

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
