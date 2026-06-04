// Copyright Epic Games, Inc. All Rights Reserved.

#include "Player/PlayerEquipmentComponent.h"

#include "Net/UnrealNetwork.h"

UPlayerEquipmentComponent::UPlayerEquipmentComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UPlayerEquipmentComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UPlayerEquipmentComponent, CurrentWeaponTag);
}

void UPlayerEquipmentComponent::RegisterEquippedWeapon(FGameplayTag WeaponTag)
{
	if (!GetOwner() || !GetOwner()->HasAuthority() || !WeaponTag.IsValid())
	{
		return;
	}

	CurrentWeaponTag = WeaponTag;
}
