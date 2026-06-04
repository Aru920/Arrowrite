// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "BaseAbilitySystemComponent.generated.h"

class UGameplayAbilityBase;

UCLASS()
class ARROWRITE_API UBaseAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	void GiveStartupAbilities(const TArray<TSubclassOf<UGameplayAbilityBase>>& StartupAbilities, int32 AbilityLevel = 1);
};
