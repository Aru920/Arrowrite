// Copyright Epic Games, Inc. All Rights Reserved.

#include "Character/BaseCharacter.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	ApplyMovementSettings();
}

void ABaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABaseCharacter, bIsSprinting);
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	ApplyMovementSettings();
}

void ABaseCharacter::SetAimingRotationMode(bool bIsAiming)
{
	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	if (!MovementComponent)
	{
		return;
	}

	bUseControllerRotationYaw = bIsAiming;
	MovementComponent->bUseControllerDesiredRotation = !bIsAiming;
	MovementComponent->bOrientRotationToMovement = !bIsAiming;
}

void ABaseCharacter::SetSprinting(bool bNewIsSprinting)
{
	if (bIsSprinting == bNewIsSprinting)
	{
		return;
	}

	bIsSprinting = bNewIsSprinting;
	ApplyMovementSettings();

	if (!HasAuthority())
	{
		ServerSetSprinting(bNewIsSprinting);
	}
}

void ABaseCharacter::ServerSetSprinting_Implementation(bool bNewIsSprinting)
{
	SetSprinting(bNewIsSprinting);
}

void ABaseCharacter::OnRep_IsSprinting()
{
	ApplyMovementSettings();
}

void ABaseCharacter::ApplyMovementSettings()
{
	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	if (!MovementComponent)
	{
		return;
	}

	MovementComponent->MaxWalkSpeed = bIsSprinting ? RunSpeed : WalkSpeed;
}
