// Copyright Epic Games, Inc. All Rights Reserved.

#include "Weapons/BaseWeapon.h"

#include "Animation/Weapons/WeaponAnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Pawn.h"
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
	DOREPLIFETIME(ABaseWeapon, bWeaponAimPoseActive);
	DOREPLIFETIME(ABaseWeapon, WeaponAnimState);
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

void ABaseWeapon::SetWeaponAimPoseActive(bool bShouldUseAimPose)
{
	if (bWeaponAimPoseActive == bShouldUseAimPose)
	{
		ApplyWeaponAimPoseActive();
		return;
	}

	bWeaponAimPoseActive = bShouldUseAimPose;
	ApplyWeaponAimPoseActive();

	const APawn* OwnerPawn = OwningPawn ? OwningPawn.Get() : Cast<APawn>(GetOwner());
	if (!HasAuthority() && OwnerPawn && OwnerPawn->IsLocallyControlled())
	{
		ServerSetWeaponAimPoseActive(bShouldUseAimPose);
	}
}

void ABaseWeapon::ApplyWeaponAimPoseActive()
{
	if (UWeaponAnimInstance* WeaponAnimInstance = Cast<UWeaponAnimInstance>(WeaponMesh ? WeaponMesh->GetAnimInstance() : nullptr))
	{
		WeaponAnimInstance->SetWeaponAimPoseActive(bWeaponAimPoseActive);
	}
}

void ABaseWeapon::ServerSetWeaponAimPoseActive_Implementation(bool bShouldUseAimPose)
{
	SetWeaponAimPoseActive(bShouldUseAimPose);
}

void ABaseWeapon::OnRep_WeaponAimPoseActive()
{
	ApplyWeaponAimPoseActive();
}

void ABaseWeapon::SetWeaponAnimState(EWeaponAnimState NewAnimState)
{
	if (WeaponAnimState == NewAnimState)
	{
		ApplyWeaponAnimState();
		return;
	}

	WeaponAnimState = NewAnimState;
	ApplyWeaponAnimState();

	const APawn* OwnerPawn = OwningPawn ? OwningPawn.Get() : Cast<APawn>(GetOwner());
	if (!HasAuthority() && OwnerPawn && OwnerPawn->IsLocallyControlled())
	{
		ServerSetWeaponAnimState(NewAnimState);
	}
}

void ABaseWeapon::ApplyWeaponAnimState()
{
	if (UWeaponAnimInstance* WeaponAnimInstance = Cast<UWeaponAnimInstance>(WeaponMesh ? WeaponMesh->GetAnimInstance() : nullptr))
	{
		WeaponAnimInstance->SetWeaponAnimState(WeaponAnimState);
	}
}

void ABaseWeapon::ServerSetWeaponAnimState_Implementation(EWeaponAnimState NewAnimState)
{
	SetWeaponAnimState(NewAnimState);
}

void ABaseWeapon::OnRep_WeaponAnimState()
{
	ApplyWeaponAnimState();
}
