// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "WeaponAnimInstance.generated.h"

UENUM(BlueprintType)
enum class EWeaponAnimState : uint8
{
	Idle,
	AimStart,
	AimHold,
	AimEnd,
	Fire
};

UCLASS()
class ARROWRITE_API UWeaponAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Animation|Weapon")
	void SetWeaponAimPoseActive(bool bShouldUseAimPose);

	UFUNCTION(BlueprintCallable, Category = "Animation|Weapon")
	void SetWeaponAnimState(EWeaponAnimState NewAnimState);

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Animation|Weapon")
	bool bUseAimPose = false;

	UPROPERTY(BlueprintReadOnly, Category = "Animation|Weapon")
	EWeaponAnimState WeaponAnimState = EWeaponAnimState::Idle;
};
