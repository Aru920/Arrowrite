// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "ScalableFloat.h"
#include "ArrowDataAsset.generated.h"

class UGameplayEffect;
class UParticleSystem;
class USoundBase;

UCLASS()
class ARROWRITE_API UArrowDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Arrow")
	FGameplayTag ArrowTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Arrow|Damage")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Arrow|Damage")
	FScalableFloat BaseDamage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Arrow|Status")
	TSubclassOf<UGameplayEffect> StatusEffectClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Arrow|FX")
	TObjectPtr<UParticleSystem> TrailFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Arrow|FX")
	TObjectPtr<UParticleSystem> WorldImpactFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Arrow|FX")
	TObjectPtr<USoundBase> WorldImpactSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Arrow|FX")
	TObjectPtr<UParticleSystem> CharacterImpactFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Arrow|FX")
	TObjectPtr<USoundBase> CharacterImpactSound;
};
