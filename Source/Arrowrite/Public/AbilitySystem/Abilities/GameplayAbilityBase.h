// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayAbilityBase.generated.h"

class AGamePlayerController;
class APlayerCharacter;
class UBaseAbilitySystemComponent;
class UPlayerAbilitySystemComponent;

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

protected:
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

	UFUNCTION(BlueprintPure, Category = "Ability")
	UBaseAbilitySystemComponent* GetBaseAbilitySystemComponentFromActorInfo() const;

	UFUNCTION(BlueprintPure, Category = "Ability|Player")
	UPlayerAbilitySystemComponent* GetPlayerAbilitySystemComponentFromActorInfo() const;

	UFUNCTION(BlueprintPure, Category = "Ability|Player")
	APlayerCharacter* GetPlayerCharacterFromActorInfo() const;

	UFUNCTION(BlueprintPure, Category = "Ability|Player")
	AGamePlayerController* GetGamePlayerControllerFromActorInfo() const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability")
	EAbilityActivationPolicy ActivationPolicy = EAbilityActivationPolicy::OnInputTriggered;
};
