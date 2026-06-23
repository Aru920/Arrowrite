// Copyright Epic Games, Inc. All Rights Reserved.

#include "AbilitySystem/Abilities/Player/PlayerGameplayAbility.h"

#include "AbilitySystem/Player/PlayerAbilitySystemComponent.h"
#include "AbilitySystem/Player/PlayerAttributeSet.h"
#include "Character/Player/PlayerCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/PlayerController.h"
#include "Player/GamePlayerController.h"
#include "Player/GamePlayerState.h"
#include "Player/PlayerEquipmentComponent.h"
#include "Projectiles/ArrowProjectile.h"
#include "Projectiles/ArrowDataAsset.h"
#include "Weapons/BaseWeapon.h"

UPlayerAbilitySystemComponent* UPlayerGameplayAbility::GetPlayerAbilitySystemComponentFromActorInfo() const
{
	return Cast<UPlayerAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo());
}

UPlayerAttributeSet* UPlayerGameplayAbility::GetPlayerAttributeSetFromActorInfo() const
{
	const FGameplayAbilityActorInfo* ActorInfo = GetCurrentActorInfo();
	const AGamePlayerState* GamePlayerState = ActorInfo ? Cast<AGamePlayerState>(ActorInfo->OwnerActor.Get()) : nullptr;
	return GamePlayerState ? GamePlayerState->GetPlayerAttributeSet() : nullptr;
}

bool UPlayerGameplayAbility::HasEnoughStamina(const float StaminaCost) const
{
	if (StaminaCost <= 0.0f)
	{
		return true;
	}

	const UPlayerAttributeSet* AttributeSet = GetPlayerAttributeSetFromActorInfo();
	return AttributeSet && AttributeSet->GetStamina() >= StaminaCost;
}

APlayerCharacter* UPlayerGameplayAbility::GetPlayerCharacterFromActorInfo() const
{
	return Cast<APlayerCharacter>(GetAvatarActorFromActorInfo());
}

APlayerController* UPlayerGameplayAbility::GetPlayerControllerFromActorInfo() const
{
	const FGameplayAbilityActorInfo* ActorInfo = GetCurrentActorInfo();
	if (APlayerController* PlayerController = ActorInfo ? Cast<APlayerController>(ActorInfo->PlayerController.Get()) : nullptr)
	{
		return PlayerController;
	}

	const APlayerCharacter* PlayerCharacter = GetPlayerCharacterFromActorInfo();
	return PlayerCharacter ? Cast<APlayerController>(PlayerCharacter->GetController()) : nullptr;
}

AGamePlayerController* UPlayerGameplayAbility::GetGamePlayerControllerFromActorInfo() const
{
	return Cast<AGamePlayerController>(GetPlayerControllerFromActorInfo());
}

AGamePlayerController* UPlayerGameplayAbility::GetLocalGamePlayerControllerFromActorInfo() const
{
	AGamePlayerController* PlayerController = GetGamePlayerControllerFromActorInfo();
	return PlayerController && PlayerController->IsLocalController() ? PlayerController : nullptr;
}

bool UPlayerGameplayAbility::CanUseLocalPlayerWidgets() const
{
	return GetLocalGamePlayerControllerFromActorInfo() != nullptr;
}

void UPlayerGameplayAbility::SetCrosshairVisible(bool bShouldBeVisible) const
{
	if (AGamePlayerController* PlayerController = GetGamePlayerControllerFromActorInfo())
	{
		PlayerController->SetCrosshairVisible(bShouldBeVisible);
	}
}

void UPlayerGameplayAbility::SetBowAimingState(bool bShouldAim) const
{
	if (APlayerCharacter* PlayerCharacter = GetPlayerCharacterFromActorInfo())
	{
		PlayerCharacter->SetBowAimPoseActive(bShouldAim);
	}

	if (ABaseWeapon* CurrentWeapon = GetCurrentWeaponFromActorInfo())
	{
		CurrentWeapon->SetWeaponAimPoseActive(bShouldAim);
	}
}

USkeletalMeshComponent* UPlayerGameplayAbility::GetPlayerMeshFromActorInfo() const
{
	const APlayerCharacter* PlayerCharacter = GetPlayerCharacterFromActorInfo();
	return PlayerCharacter ? PlayerCharacter->GetMesh() : nullptr;
}

UPlayerEquipmentComponent* UPlayerGameplayAbility::GetPlayerEquipmentComponentFromActorInfo() const
{
	const APlayerCharacter* PlayerCharacter = GetPlayerCharacterFromActorInfo();
	return PlayerCharacter ? PlayerCharacter->GetEquipmentComponent() : nullptr;
}

UArrowDataAsset* UPlayerGameplayAbility::GetSelectedArrowDataFromActorInfo() const
{
	const UPlayerEquipmentComponent* EquipmentComponent = GetPlayerEquipmentComponentFromActorInfo();
	return EquipmentComponent ? EquipmentComponent->GetSelectedArrowData() : nullptr;
}

void UPlayerGameplayAbility::ApplySelectedArrowDataToProjectile(AArrowProjectile* Projectile) const
{
	if (!IsValid(Projectile))
	{
		return;
	}

	Projectile->SetArrowData(GetSelectedArrowDataFromActorInfo());
}

ABaseWeapon* UPlayerGameplayAbility::GetCurrentWeaponFromActorInfo() const
{
	const UPlayerEquipmentComponent* EquipmentComponent = GetPlayerEquipmentComponentFromActorInfo();
	ABaseWeapon* CurrentWeapon = EquipmentComponent ? EquipmentComponent->GetCurrentWeapon() : nullptr;
	return IsValid(CurrentWeapon) ? CurrentWeapon : nullptr;
}
