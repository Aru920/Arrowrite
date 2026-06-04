// Copyright Epic Games, Inc. All Rights Reserved.

#include "Weapons/BaseWeapon.h"

#include "Components/SkeletalMeshComponent.h"
#include "Net/UnrealNetwork.h"

ABaseWeapon::ABaseWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SetReplicateMovement(false);

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABaseWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABaseWeapon, OwningPawn);
}

void ABaseWeapon::SetOwningPawn(APawn* InOwningPawn)
{
	if (!InOwningPawn)
	{
		return;
	}

	OwningPawn = InOwningPawn;
	SetOwner(InOwningPawn);
	SetInstigator(InOwningPawn);
}
