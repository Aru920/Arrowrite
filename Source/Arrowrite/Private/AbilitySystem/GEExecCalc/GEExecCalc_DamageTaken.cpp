// Copyright Epic Games, Inc. All Rights Reserved.

#include "AbilitySystem/GEExecCalc/GEExecCalc_DamageTaken.h"

#include "AbilitySystem/Player/PlayerAttributeSet.h"
#include "Tags/GameplayTags.h"

struct FArrowriteDamageCapture
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(DamageTaken)

	FArrowriteDamageCapture()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UPlayerAttributeSet, DamageTaken, Target, false)
	}
};

static const FArrowriteDamageCapture& GetArrowriteDamageCapture()
{
	static FArrowriteDamageCapture DamageCapture;
	return DamageCapture;
}

UGEExecCalc_DamageTaken::UGEExecCalc_DamageTaken()
{
	RelevantAttributesToCapture.Add(GetArrowriteDamageCapture().DamageTakenDef);
}

void UGEExecCalc_DamageTaken::Execute_Implementation(
	const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput
) const
{
	const FGameplayEffectSpec& EffectSpec = ExecutionParams.GetOwningSpec();
	const float BaseDamage = EffectSpec.GetSetByCallerMagnitude(
		ArrowriteGameplayTags::Shared_SetByCaller_BaseDamage,
		false,
		0.0f
	);

	const float FinalDamage = FMath::Max(BaseDamage, 0.0f);
	if (FinalDamage <= 0.0f)
	{
		return;
	}

	OutExecutionOutput.AddOutputModifier(
		FGameplayModifierEvaluatedData(
			GetArrowriteDamageCapture().DamageTakenProperty,
			EGameplayModOp::Override,
			FinalDamage
		)
	);
}
