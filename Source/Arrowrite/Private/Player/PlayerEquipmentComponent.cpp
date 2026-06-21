// Copyright Epic Games, Inc. All Rights Reserved.

#include "Player/PlayerEquipmentComponent.h"

#include "Net/UnrealNetwork.h"
#include "Projectiles/ArrowDataAsset.h"
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
	DOREPLIFETIME(UPlayerEquipmentComponent, SelectedArrowTag);
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

void UPlayerEquipmentComponent::DestroyCarriedWeapons()
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}

	for (FCarriedWeaponEntry& CarriedWeapon : CarriedWeapons)
	{
		if (ABaseWeapon* Weapon = CarriedWeapon.Weapon.Get())
		{
			Weapon->Destroy();
		}
	}

	CarriedWeapons.Empty();
	CurrentWeaponTag = FGameplayTag();
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

void UPlayerEquipmentComponent::SelectArrowByTag(FGameplayTag ArrowTag)
{
	if (!ArrowTag.IsValid() || !GetArrowDataByTag(ArrowTag))
	{
		return;
	}

	if (SelectedArrowTag == ArrowTag)
	{
		BroadcastSelectedArrowChanged();
		return;
	}

	SelectedArrowTag = ArrowTag;
	BroadcastSelectedArrowChanged();

	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		ServerSelectArrowByTag(ArrowTag);
	}
}

UArrowDataAsset* UPlayerEquipmentComponent::GetSelectedArrowData() const
{
	UArrowDataAsset* SelectedData = GetArrowDataByTag(SelectedArrowTag);
	if (SelectedData)
	{
		return SelectedData;
	}

	return AvailableArrowData.Num() > 0 ? AvailableArrowData[0] : nullptr;
}

UArrowDataAsset* UPlayerEquipmentComponent::GetArrowDataByTag(FGameplayTag ArrowTag) const
{
	if (!ArrowTag.IsValid())
	{
		return nullptr;
	}

	for (UArrowDataAsset* ArrowData : AvailableArrowData)
	{
		if (ArrowData && ArrowData->ArrowTag == ArrowTag)
		{
			return ArrowData;
		}
	}

	return nullptr;
}

void UPlayerEquipmentComponent::ServerSelectArrowByTag_Implementation(FGameplayTag ArrowTag)
{
	SelectArrowByTag(ArrowTag);
}

void UPlayerEquipmentComponent::OnRep_SelectedArrowTag()
{
	BroadcastSelectedArrowChanged();
}

void UPlayerEquipmentComponent::BroadcastSelectedArrowChanged()
{
	UArrowDataAsset* SelectedArrowData = GetSelectedArrowData();
	OnSelectedArrowChanged(SelectedArrowData);
	OnSelectedArrowDataChanged.Broadcast(SelectedArrowData);
}
