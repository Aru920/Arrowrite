// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MenuTravelBlueprintLibrary.generated.h"

UCLASS()
class ARROWRITE_API UMenuTravelBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Menu|Travel", meta = (WorldContext = "WorldContextObject"))
	static void HostListenServer(const UObject* WorldContextObject, FName MapName = TEXT("ThirdPersonMap"));

	UFUNCTION(BlueprintCallable, Category = "Menu|Travel", meta = (WorldContext = "WorldContextObject"))
	static void HostListenServerWithPlayerName(const UObject* WorldContextObject, const FString& PlayerName, FName MapName = TEXT("ThirdPersonMap"));

	UFUNCTION(BlueprintCallable, Category = "Menu|Travel", meta = (WorldContext = "WorldContextObject"))
	static bool JoinServerByAddress(const UObject* WorldContextObject, const FString& ServerAddress);

	UFUNCTION(BlueprintCallable, Category = "Menu|Travel", meta = (WorldContext = "WorldContextObject"))
	static bool JoinServerByAddressWithPlayerName(const UObject* WorldContextObject, const FString& ServerAddress, const FString& PlayerName);
};
