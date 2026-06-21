// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "GamePlayerState.generated.h"

class UPlayerAbilitySystemComponent;
class UPlayerAttributeSet;

UCLASS()
class ARROWRITE_API AGamePlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AGamePlayerState();

	UPlayerAbilitySystemComponent* GetPlayerAbilitySystemComponent() const { return AbilitySystemComponent; }
	UPlayerAttributeSet* GetPlayerAttributeSet() const { return AttributeSet; }

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void ResetCombatStateForRespawn();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ability System")
	TObjectPtr<UPlayerAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ability System")
	TObjectPtr<UPlayerAttributeSet> AttributeSet;
};
