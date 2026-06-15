// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CombatBlueprintLibrary.generated.h"

UENUM(BlueprintType)
enum class EHitReactDirection : uint8
{
	Front,
	Back,
	Left,
	Right
};

UCLASS()
class ARROWRITE_API UCombatBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Combat")
	static EHitReactDirection CalculateHitReactDirection(AActor* TargetActor, AActor* SourceActor);

	UFUNCTION(BlueprintPure, Category = "Combat")
	static EHitReactDirection CalculateHitReactDirectionFromLocation(AActor* TargetActor, FVector SourceLocation);
};
