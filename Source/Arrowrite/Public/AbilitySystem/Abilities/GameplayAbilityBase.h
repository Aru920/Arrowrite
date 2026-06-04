// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayTagContainer.h"
#include "GameplayAbilityBase.generated.h"

class UBaseAbilitySystemComponent;

UENUM(BlueprintType)
enum class EAbilityActivationPolicy : uint8
{
	OnInputTriggered,
	OnInputHeld,
	OnGiven
};

UCLASS()
class ARROWRITE_API UGameplayAbilityBase : public UGameplayAbility
{
	GENERATED_BODY()

public:
	EAbilityActivationPolicy GetActivationPolicy() const { return ActivationPolicy; }
	FGameplayTag GetInputTag() const { return InputTag; }

protected:
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

	UFUNCTION(BlueprintPure, Category = "Ability")
	UBaseAbilitySystemComponent* GetBaseAbilitySystemComponentFromActorInfo() const;

	UFUNCTION(BlueprintPure, Category = "Ability")
	AActor* GetAvatarActorFromAbilityActorInfo() const;

	UFUNCTION(BlueprintPure, Category = "Ability")
	AActor* GetOwnerActorFromAbilityActorInfo() const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability")
	EAbilityActivationPolicy ActivationPolicy = EAbilityActivationPolicy::OnInputTriggered;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability|Input")
	FGameplayTag InputTag;
};
