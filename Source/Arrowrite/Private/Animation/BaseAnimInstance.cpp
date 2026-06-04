// Copyright Epic Games, Inc. All Rights Reserved.

#include "Animation/BaseAnimInstance.h"

#include "Character/BaseCharacter.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void UBaseAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	CacheCharacterReferences();
}

void UBaseAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!Character || !CharacterMovement)
	{
		CacheCharacterReferences();
	}

	UpdateMovementValues();
}

void UBaseAnimInstance::CacheCharacterReferences()
{
	Character = Cast<ABaseCharacter>(TryGetPawnOwner());
	CharacterMovement = Character ? Character->GetCharacterMovement() : nullptr;
}

void UBaseAnimInstance::UpdateMovementValues()
{
	if (!Character || !CharacterMovement)
	{
		return;
	}

	const FVector Velocity = Character->GetVelocity();
	const FVector GroundVelocity(Velocity.X, Velocity.Y, 0.0f);

	GroundSpeed = GroundVelocity.Size();
	Direction = CalculateMovementDirection(Character, GroundVelocity);
}

float UBaseAnimInstance::CalculateMovementDirection(const ACharacter* InCharacter, const FVector& Velocity)
{
	if (!InCharacter || Velocity.IsNearlyZero())
	{
		return 0.0f;
	}

	const FVector Forward = InCharacter->GetActorForwardVector().GetSafeNormal2D();
	const FVector Right = InCharacter->GetActorRightVector().GetSafeNormal2D();
	const FVector MovementDirection = Velocity.GetSafeNormal2D();

	const float ForwardDot = FVector::DotProduct(Forward, MovementDirection);
	const float RightDot = FVector::DotProduct(Right, MovementDirection);

	return FMath::RadiansToDegrees(FMath::Atan2(RightDot, ForwardDot));
}
