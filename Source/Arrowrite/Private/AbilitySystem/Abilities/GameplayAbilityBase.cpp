// Copyright Epic Games, Inc. All Rights Reserved.

#include "AbilitySystem/Abilities/GameplayAbilityBase.h"

#include "AbilitySystem/BaseAbilitySystemComponent.h"
#include "AbilitySystem/Player/PlayerAbilitySystemComponent.h"
#include "Character/Player/PlayerCharacter.h"
#include "Player/GamePlayerController.h"

void UGameplayAbilityBase::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	if (ActivationPolicy == EAbilityActivationPolicy::OnGiven && ActorInfo && ActorInfo->AbilitySystemComponent.IsValid() && !Spec.IsActive())
	{
		ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle);
	}
}

UBaseAbilitySystemComponent* UGameplayAbilityBase::GetBaseAbilitySystemComponentFromActorInfo() const
{
	return Cast<UBaseAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo());
}

UPlayerAbilitySystemComponent* UGameplayAbilityBase::GetPlayerAbilitySystemComponentFromActorInfo() const
{
	return Cast<UPlayerAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo());
}

APlayerCharacter* UGameplayAbilityBase::GetPlayerCharacterFromActorInfo() const
{
	return Cast<APlayerCharacter>(GetAvatarActorFromActorInfo());
}

AGamePlayerController* UGameplayAbilityBase::GetGamePlayerControllerFromActorInfo() const
{
	const FGameplayAbilityActorInfo* ActorInfo = GetCurrentActorInfo();
	return ActorInfo ? Cast<AGamePlayerController>(ActorInfo->PlayerController.Get()) : nullptr;
}
