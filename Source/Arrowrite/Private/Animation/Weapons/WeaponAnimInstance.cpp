// Copyright Epic Games, Inc. All Rights Reserved.

#include "Animation/Weapons/WeaponAnimInstance.h"

void UWeaponAnimInstance::SetWeaponAimPoseActive(bool bShouldUseAimPose)
{
	bUseAimPose = bShouldUseAimPose;
}

void UWeaponAnimInstance::SetWeaponAnimState(EWeaponAnimState NewAnimState)
{
	WeaponAnimState = NewAnimState;
}
