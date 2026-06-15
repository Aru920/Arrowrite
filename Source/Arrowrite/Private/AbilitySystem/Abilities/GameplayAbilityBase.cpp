// Copyright Epic Games, Inc. All Rights Reserved.

#include "AbilitySystem/Abilities/GameplayAbilityBase.h"

#include "AbilitySystem/BaseAbilitySystemComponent.h"

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

AActor* UGameplayAbilityBase::GetAvatarActorFromAbilityActorInfo() const
{
	return GetAvatarActorFromActorInfo();
}

AActor* UGameplayAbilityBase::GetOwnerActorFromAbilityActorInfo() const
{
	return GetOwningActorFromActorInfo();
}

bool UGameplayAbilityBase::IsAvatarActorAuthority() const
{
	const AActor* AvatarActor = GetAvatarActorFromActorInfo();
	return AvatarActor && AvatarActor->HasAuthority();
}
