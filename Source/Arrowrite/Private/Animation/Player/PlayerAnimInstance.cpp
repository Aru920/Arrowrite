// Copyright Epic Games, Inc. All Rights Reserved.

#include "Animation/Player/PlayerAnimInstance.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Character/BaseCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "Tags/GameplayTags.h"

void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	UpdateAimValues();
}

void UPlayerAnimInstance::SetBowAimPoseActive(bool bShouldUseBowAimPose)
{
	bUseBowAimPose = bShouldUseBowAimPose;
}

void UPlayerAnimInstance::UpdateAimValues()
{
	bIsAiming = false;
	AimYaw = 0.0f;
	AimPitch = 0.0f;

	if (!Character)
	{
		return;
	}

	const UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Character);
	bIsAiming = AbilitySystemComponent && AbilitySystemComponent->HasMatchingGameplayTag(ArrowriteGameplayTags::State_Aiming);

	const FRotator AimRotation = Character->GetBaseAimRotation();
	const FRotator ActorRotation = Character->GetActorRotation();
	const FRotator DeltaRotation = UKismetMathLibrary::NormalizedDeltaRotator(AimRotation, ActorRotation);

	AimYaw = DeltaRotation.Yaw;
	AimPitch = DeltaRotation.Pitch;
}
