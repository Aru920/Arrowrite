// Copyright Epic Games, Inc. All Rights Reserved.

#include "BlueprintFunctionLibraries/CombatBlueprintLibrary.h"

EHitReactDirection UCombatBlueprintLibrary::CalculateHitReactDirection(AActor* TargetActor, AActor* SourceActor)
{
	if (!SourceActor)
	{
		return EHitReactDirection::Front;
	}

	return CalculateHitReactDirectionFromLocation(TargetActor, SourceActor->GetActorLocation());
}

EHitReactDirection UCombatBlueprintLibrary::CalculateHitReactDirectionFromLocation(AActor* TargetActor, FVector SourceLocation)
{
	if (!TargetActor)
	{
		return EHitReactDirection::Front;
	}

	const FVector ToSource = (SourceLocation - TargetActor->GetActorLocation()).GetSafeNormal2D();
	if (ToSource.IsNearlyZero())
	{
		return EHitReactDirection::Front;
	}

	const FVector Forward = TargetActor->GetActorForwardVector().GetSafeNormal2D();
	const FVector Right = TargetActor->GetActorRightVector().GetSafeNormal2D();

	const float ForwardDot = FVector::DotProduct(ToSource, Forward);
	const float RightDot = FVector::DotProduct(ToSource, Right);

	if (FMath::Abs(ForwardDot) >= FMath::Abs(RightDot))
	{
		return ForwardDot >= 0.0f ? EHitReactDirection::Front : EHitReactDirection::Back;
	}

	return RightDot >= 0.0f ? EHitReactDirection::Right : EHitReactDirection::Left;
}
