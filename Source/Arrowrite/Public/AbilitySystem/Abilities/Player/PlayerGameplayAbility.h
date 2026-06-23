// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/GameplayAbilityBase.h"
#include "PlayerGameplayAbility.generated.h"

class AGamePlayerController;
class APlayerController;
class APlayerCharacter;
class AArrowProjectile;
class ABaseWeapon;
class UArrowDataAsset;
class UPlayerAbilitySystemComponent;
class UPlayerAttributeSet;
class UPlayerEquipmentComponent;
class USkeletalMeshComponent;

UCLASS()
class ARROWRITE_API UPlayerGameplayAbility : public UGameplayAbilityBase
{
	GENERATED_BODY()

protected:
	UFUNCTION(BlueprintPure, Category = "Ability|Player")
	UPlayerAbilitySystemComponent* GetPlayerAbilitySystemComponentFromActorInfo() const;

	UFUNCTION(BlueprintPure, Category = "Ability|Player")
	UPlayerAttributeSet* GetPlayerAttributeSetFromActorInfo() const;

	UFUNCTION(BlueprintPure, Category = "Ability|Player|Stamina")
	bool HasEnoughStamina(float StaminaCost) const;

	UFUNCTION(BlueprintPure, Category = "Ability|Player")
	APlayerCharacter* GetPlayerCharacterFromActorInfo() const;

	UFUNCTION(BlueprintPure, Category = "Ability|Player")
	APlayerController* GetPlayerControllerFromActorInfo() const;

	UFUNCTION(BlueprintPure, Category = "Ability|Player")
	AGamePlayerController* GetGamePlayerControllerFromActorInfo() const;

	UFUNCTION(BlueprintPure, Category = "Ability|Player|UI")
	AGamePlayerController* GetLocalGamePlayerControllerFromActorInfo() const;

	UFUNCTION(BlueprintPure, Category = "Ability|Player|UI")
	bool CanUseLocalPlayerWidgets() const;

	UFUNCTION(BlueprintCallable, Category = "Ability|Player|UI")
	void SetCrosshairVisible(bool bShouldBeVisible) const;

	UFUNCTION(BlueprintCallable, Category = "Ability|Player|Bow")
	void SetBowAimingState(bool bShouldAim) const;

	UFUNCTION(BlueprintPure, Category = "Ability|Player")
	USkeletalMeshComponent* GetPlayerMeshFromActorInfo() const;

	UFUNCTION(BlueprintPure, Category = "Ability|Player")
	UPlayerEquipmentComponent* GetPlayerEquipmentComponentFromActorInfo() const;

	UFUNCTION(BlueprintPure, Category = "Ability|Player|Arrows")
	UArrowDataAsset* GetSelectedArrowDataFromActorInfo() const;

	UFUNCTION(BlueprintCallable, Category = "Ability|Player|Arrows")
	void ApplySelectedArrowDataToProjectile(AArrowProjectile* Projectile) const;

	UFUNCTION(BlueprintPure, Category = "Ability|Player")
	ABaseWeapon* GetCurrentWeaponFromActorInfo() const;
};
