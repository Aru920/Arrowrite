// Copyright Epic Games, Inc. All Rights Reserved.

#include "Player/PlayerEquipmentComponent.h"

#include "Net/UnrealNetwork.h"
#include "Weapons/BaseWeapon.h"
#include "Weapons/WeaponDataAsset.h"

UPlayerEquipmentComponent::UPlayerEquipmentComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UPlayerEquipmentComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UPlayerEquipmentComponent, CurrentWeaponTag);
	DOREPLIFETIME(UPlayerEquipmentComponent, CarriedWeapons);
}

void UPlayerEquipmentComponent::RegisterEquippedWeapon(ABaseWeapon* Weapon)
{
	if (!GetOwner() || !GetOwner()->HasAuthority() || !Weapon)
	{
		return;
	}

	if (const UWeaponDataAsset* WeaponData = Weapon->GetWeaponData())
	{
		if (!WeaponData->WeaponTag.IsValid())
		{
			return;
		}

		Weapon->SetOwningPawn(Cast<APawn>(GetOwner()));

		if (FCarriedWeaponEntry* ExistingEntry = CarriedWeapons.FindByPredicate(
			[WeaponData](const FCarriedWeaponEntry& Entry)
			{
				return Entry.WeaponTag == WeaponData->WeaponTag;
			}))
		{
			ExistingEntry->Weapon = Weapon;
		}
		else
		{
			FCarriedWeaponEntry NewEntry;
			NewEntry.WeaponTag = WeaponData->WeaponTag;
			NewEntry.Weapon = Weapon;
			CarriedWeapons.Add(NewEntry);
		}

		CurrentWeaponTag = WeaponData->WeaponTag;
	}
}

ABaseWeapon* UPlayerEquipmentComponent::GetWeaponByTag(FGameplayTag WeaponTag) const
{
	if (!WeaponTag.IsValid())
	{
		return nullptr;
	}

	const FCarriedWeaponEntry* FoundEntry = CarriedWeapons.FindByPredicate(
		[WeaponTag](const FCarriedWeaponEntry& Entry)
		{
			return Entry.WeaponTag == WeaponTag;
		});

	if (FoundEntry)
	{
		return FoundEntry->Weapon.Get();
	}

	return nullptr;
}

ABaseWeapon* UPlayerEquipmentComponent::GetCurrentWeapon() const
{
	return GetWeaponByTag(CurrentWeaponTag);
}
