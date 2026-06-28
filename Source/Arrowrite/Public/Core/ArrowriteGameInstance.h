// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "ArrowriteGameInstance.generated.h"

UCLASS()
class ARROWRITE_API UArrowriteGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Player")
	void SetDesiredPlayerName(const FString& PlayerName);

	UFUNCTION(BlueprintPure, Category = "Player")
	const FString& GetDesiredPlayerName() const { return DesiredPlayerName; }

	UFUNCTION(BlueprintPure, Category = "Player")
	static FString MakeSafePlayerName(const FString& PlayerName);

private:
	UPROPERTY()
	FString DesiredPlayerName = TEXT("Player");
};
