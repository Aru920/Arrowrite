// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CombatUIInterface.generated.h"

class UPlayerCombatUIComponent;

UINTERFACE(MinimalAPI, Blueprintable)
class UCombatUIInterface : public UInterface
{
	GENERATED_BODY()
};

class ARROWRITE_API ICombatUIInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "UI")
	UPlayerCombatUIComponent* GetPlayerCombatUIComponent() const;
};
