// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WeaponTypes.generated.h"

class UGameplayAbilityBase;

USTRUCT(BlueprintType)
struct FWeaponAbilitySet
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Abilities")
	TArray<TSubclassOf<UGameplayAbilityBase>> GrantedAbilities;
};
