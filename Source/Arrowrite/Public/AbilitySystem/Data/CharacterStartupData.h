// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CharacterStartupData.generated.h"

class UBaseAbilitySystemComponent;
class UGameplayAbilityBase;

UCLASS()
class ARROWRITE_API UCharacterStartupData : public UDataAsset
{
	GENERATED_BODY()

public:
	void GiveToAbilitySystemComponent(UBaseAbilitySystemComponent* AbilitySystemComponent, int32 AbilityLevel = 1) const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TArray<TSubclassOf<UGameplayAbilityBase>> StartupAbilities;
};
