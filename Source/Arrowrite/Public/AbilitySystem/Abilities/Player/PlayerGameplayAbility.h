// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/GameplayAbilityBase.h"
#include "PlayerGameplayAbility.generated.h"

class AGamePlayerController;
class APlayerCharacter;
class UPlayerAbilitySystemComponent;
class USkeletalMeshComponent;

UCLASS()
class ARROWRITE_API UPlayerGameplayAbility : public UGameplayAbilityBase
{
	GENERATED_BODY()

protected:
	UFUNCTION(BlueprintPure, Category = "Ability|Player")
	UPlayerAbilitySystemComponent* GetPlayerAbilitySystemComponentFromActorInfo() const;

	UFUNCTION(BlueprintPure, Category = "Ability|Player")
	APlayerCharacter* GetPlayerCharacterFromActorInfo() const;

	UFUNCTION(BlueprintPure, Category = "Ability|Player")
	AGamePlayerController* GetGamePlayerControllerFromActorInfo() const;

	UFUNCTION(BlueprintPure, Category = "Ability|Player")
	USkeletalMeshComponent* GetPlayerMeshFromActorInfo() const;
};
