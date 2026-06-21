// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerCombatUIComponent.generated.h"

class UPlayerAbilitySystemComponent;
class UPlayerAttributeSet;
struct FOnAttributeChangeData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCombatAttributeChangedDelegate, float, NewValue, float, OldValue);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ARROWRITE_API UPlayerCombatUIComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPlayerCombatUIComponent();

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void InitializeWithAbilitySystem(UPlayerAbilitySystemComponent* InAbilitySystemComponent, UPlayerAttributeSet* InAttributeSet);
	void ClearAbilitySystemBindings();

	UFUNCTION(BlueprintPure, Category = "Combat UI|Attributes")
	float GetHealth() const;

	UFUNCTION(BlueprintPure, Category = "Combat UI|Attributes")
	float GetMaxHealth() const;

	UFUNCTION(BlueprintPure, Category = "Combat UI|Attributes")
	float GetHealthPercent() const;

	UFUNCTION(BlueprintPure, Category = "Combat UI|Attributes")
	float GetStamina() const;

	UFUNCTION(BlueprintPure, Category = "Combat UI|Attributes")
	float GetMaxStamina() const;

	UFUNCTION(BlueprintPure, Category = "Combat UI|Attributes")
	float GetStaminaPercent() const;

	UFUNCTION(BlueprintCallable, Category = "Combat UI|Attributes")
	void BroadcastInitialAttributeValues();

	UPROPERTY(BlueprintAssignable, Category = "Combat UI|Attributes")
	FCombatAttributeChangedDelegate OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "Combat UI|Attributes")
	FCombatAttributeChangedDelegate OnMaxHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "Combat UI|Attributes")
	FCombatAttributeChangedDelegate OnStaminaChanged;

	UPROPERTY(BlueprintAssignable, Category = "Combat UI|Attributes")
	FCombatAttributeChangedDelegate OnMaxStaminaChanged;

private:
	void BindAttributeDelegates();
	void HandleHealthChanged(const FOnAttributeChangeData& Data);
	void HandleMaxHealthChanged(const FOnAttributeChangeData& Data);
	void HandleStaminaChanged(const FOnAttributeChangeData& Data);
	void HandleMaxStaminaChanged(const FOnAttributeChangeData& Data);

	UPROPERTY(Transient)
	TObjectPtr<UPlayerAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(Transient)
	TObjectPtr<UPlayerAttributeSet> AttributeSet;

	FDelegateHandle HealthChangedDelegateHandle;
	FDelegateHandle MaxHealthChangedDelegateHandle;
	FDelegateHandle StaminaChangedDelegateHandle;
	FDelegateHandle MaxStaminaChangedDelegateHandle;

	bool bAttributeDelegatesBound = false;
};
