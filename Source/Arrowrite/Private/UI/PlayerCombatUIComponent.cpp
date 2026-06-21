// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/PlayerCombatUIComponent.h"

#include "AbilitySystem/Player/PlayerAbilitySystemComponent.h"
#include "AbilitySystem/Player/PlayerAttributeSet.h"
#include "GameplayEffectTypes.h"

UPlayerCombatUIComponent::UPlayerCombatUIComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPlayerCombatUIComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	ClearAbilitySystemBindings();

	Super::EndPlay(EndPlayReason);
}

void UPlayerCombatUIComponent::InitializeWithAbilitySystem(UPlayerAbilitySystemComponent* InAbilitySystemComponent, UPlayerAttributeSet* InAttributeSet)
{
	if (AbilitySystemComponent == InAbilitySystemComponent && AttributeSet == InAttributeSet && bAttributeDelegatesBound)
	{
		BroadcastInitialAttributeValues();
		return;
	}

	ClearAbilitySystemBindings();

	AbilitySystemComponent = InAbilitySystemComponent;
	AttributeSet = InAttributeSet;

	BindAttributeDelegates();
	BroadcastInitialAttributeValues();
}

void UPlayerCombatUIComponent::ClearAbilitySystemBindings()
{
	if (!bAttributeDelegatesBound || !AbilitySystemComponent || !AttributeSet)
	{
		bAttributeDelegatesBound = false;
		return;
	}

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UPlayerAttributeSet::GetHealthAttribute()).Remove(HealthChangedDelegateHandle);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UPlayerAttributeSet::GetMaxHealthAttribute()).Remove(MaxHealthChangedDelegateHandle);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UPlayerAttributeSet::GetStaminaAttribute()).Remove(StaminaChangedDelegateHandle);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UPlayerAttributeSet::GetMaxStaminaAttribute()).Remove(MaxStaminaChangedDelegateHandle);

	bAttributeDelegatesBound = false;
}

float UPlayerCombatUIComponent::GetHealth() const
{
	return AttributeSet ? AttributeSet->GetHealth() : 0.0f;
}

float UPlayerCombatUIComponent::GetMaxHealth() const
{
	return AttributeSet ? AttributeSet->GetMaxHealth() : 0.0f;
}

float UPlayerCombatUIComponent::GetHealthPercent() const
{
	const float MaxHealth = GetMaxHealth();
	return MaxHealth > 0.0f ? GetHealth() / MaxHealth : 0.0f;
}

float UPlayerCombatUIComponent::GetStamina() const
{
	return AttributeSet ? AttributeSet->GetStamina() : 0.0f;
}

float UPlayerCombatUIComponent::GetMaxStamina() const
{
	return AttributeSet ? AttributeSet->GetMaxStamina() : 0.0f;
}

float UPlayerCombatUIComponent::GetStaminaPercent() const
{
	const float MaxStamina = GetMaxStamina();
	return MaxStamina > 0.0f ? GetStamina() / MaxStamina : 0.0f;
}

void UPlayerCombatUIComponent::BroadcastInitialAttributeValues()
{
	OnHealthChanged.Broadcast(GetHealth(), GetHealth());
	OnMaxHealthChanged.Broadcast(GetMaxHealth(), GetMaxHealth());
	OnStaminaChanged.Broadcast(GetStamina(), GetStamina());
	OnMaxStaminaChanged.Broadcast(GetMaxStamina(), GetMaxStamina());
}

void UPlayerCombatUIComponent::BindAttributeDelegates()
{
	if (bAttributeDelegatesBound || !AbilitySystemComponent || !AttributeSet)
	{
		return;
	}

	HealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UPlayerAttributeSet::GetHealthAttribute()).AddUObject(this, &ThisClass::HandleHealthChanged);
	MaxHealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UPlayerAttributeSet::GetMaxHealthAttribute()).AddUObject(this, &ThisClass::HandleMaxHealthChanged);
	StaminaChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UPlayerAttributeSet::GetStaminaAttribute()).AddUObject(this, &ThisClass::HandleStaminaChanged);
	MaxStaminaChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UPlayerAttributeSet::GetMaxStaminaAttribute()).AddUObject(this, &ThisClass::HandleMaxStaminaChanged);

	bAttributeDelegatesBound = true;
}

void UPlayerCombatUIComponent::HandleHealthChanged(const FOnAttributeChangeData& Data)
{
	OnHealthChanged.Broadcast(Data.NewValue, Data.OldValue);
}

void UPlayerCombatUIComponent::HandleMaxHealthChanged(const FOnAttributeChangeData& Data)
{
	OnMaxHealthChanged.Broadcast(Data.NewValue, Data.OldValue);
}

void UPlayerCombatUIComponent::HandleStaminaChanged(const FOnAttributeChangeData& Data)
{
	OnStaminaChanged.Broadcast(Data.NewValue, Data.OldValue);
}

void UPlayerCombatUIComponent::HandleMaxStaminaChanged(const FOnAttributeChangeData& Data)
{
	OnMaxStaminaChanged.Broadcast(Data.NewValue, Data.OldValue);
}
