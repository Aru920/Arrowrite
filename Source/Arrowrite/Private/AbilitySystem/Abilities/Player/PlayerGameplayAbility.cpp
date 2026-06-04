// Copyright Epic Games, Inc. All Rights Reserved.

#include "AbilitySystem/Abilities/Player/PlayerGameplayAbility.h"

#include "AbilitySystem/Player/PlayerAbilitySystemComponent.h"
#include "Character/Player/PlayerCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Player/GamePlayerController.h"

UPlayerAbilitySystemComponent* UPlayerGameplayAbility::GetPlayerAbilitySystemComponentFromActorInfo() const
{
	return Cast<UPlayerAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo());
}

APlayerCharacter* UPlayerGameplayAbility::GetPlayerCharacterFromActorInfo() const
{
	return Cast<APlayerCharacter>(GetAvatarActorFromActorInfo());
}

AGamePlayerController* UPlayerGameplayAbility::GetGamePlayerControllerFromActorInfo() const
{
	const FGameplayAbilityActorInfo* ActorInfo = GetCurrentActorInfo();
	return ActorInfo ? Cast<AGamePlayerController>(ActorInfo->PlayerController.Get()) : nullptr;
}

USkeletalMeshComponent* UPlayerGameplayAbility::GetPlayerMeshFromActorInfo() const
{
	const APlayerCharacter* PlayerCharacter = GetPlayerCharacterFromActorInfo();
	return PlayerCharacter ? PlayerCharacter->GetMesh() : nullptr;
}
