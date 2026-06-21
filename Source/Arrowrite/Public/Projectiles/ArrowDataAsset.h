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
class UStaticMesh;
class UTexture2D;

UCLASS(BlueprintType)
class ARROWRITE_API UArrowDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Arrow")
	FGameplayTag ArrowTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Arrow")
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Arrow|UI")
	FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Arrow|UI")
	TObjectPtr<UTexture2D> Icon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Arrow|Visual")
	TObjectPtr<UStaticMesh> ProjectileMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Arrow|Visual")
	TObjectPtr<UStaticMesh> PreviewMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Arrow|Visual")
	TObjectPtr<UParticleSystem> EquippedTipFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Arrow|Damage")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Arrow|Damage")
	FScalableFloat BaseDamage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Arrow|Cost", meta = (ClampMin = "0.0"))
	float StaminaCost = 10.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Arrow|Status")
	TSubclassOf<UGameplayEffect> StatusEffectClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Arrow|Status")
	FScalableFloat StatusDamage;

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
