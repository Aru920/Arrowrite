// Copyright Epic Games, Inc. All Rights Reserved.

#include "BlueprintFunctionLibraries/MenuTravelBlueprintLibrary.h"

#include "Core/ArrowriteGameInstance.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

namespace
{
	FString MakePlayerNameTravelOption(const FString& PlayerName)
	{
		return FString::Printf(TEXT("Name=%s"), *UArrowriteGameInstance::MakeSafePlayerName(PlayerName));
	}

	FString AppendTravelOption(const FString& TravelTarget, const FString& TravelOption)
	{
		return FString::Printf(TEXT("%s?%s"), *TravelTarget, *TravelOption);
	}

	void SaveDesiredPlayerName(const UObject* WorldContextObject, const FString& PlayerName)
	{
		const UWorld* World = WorldContextObject ? WorldContextObject->GetWorld() : nullptr;
		if (!World)
		{
			return;
		}

		if (UArrowriteGameInstance* GameInstance = World->GetGameInstance<UArrowriteGameInstance>())
		{
			GameInstance->SetDesiredPlayerName(PlayerName);
		}
	}
}

void UMenuTravelBlueprintLibrary::HostListenServer(const UObject* WorldContextObject, FName MapName)
{
	if (!WorldContextObject || MapName.IsNone())
	{
		return;
	}

	UGameplayStatics::OpenLevel(WorldContextObject, MapName, true, TEXT("listen"));
}

void UMenuTravelBlueprintLibrary::HostListenServerWithPlayerName(const UObject* WorldContextObject, const FString& PlayerName, FName MapName)
{
	if (!WorldContextObject || MapName.IsNone())
	{
		return;
	}

	SaveDesiredPlayerName(WorldContextObject, PlayerName);
	const FString Options = FString::Printf(TEXT("listen?%s"), *MakePlayerNameTravelOption(PlayerName));
	UGameplayStatics::OpenLevel(WorldContextObject, MapName, true, Options);
}

bool UMenuTravelBlueprintLibrary::JoinServerByAddress(const UObject* WorldContextObject, const FString& ServerAddress)
{
	if (!WorldContextObject)
	{
		return false;
	}

	const FString TrimmedServerAddress = ServerAddress.TrimStartAndEnd();
	if (TrimmedServerAddress.IsEmpty())
	{
		return false;
	}

	UWorld* World = WorldContextObject->GetWorld();
	if (!World || World->bIsTearingDown)
	{
		return false;
	}

	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!PlayerController)
	{
		return false;
	}

	PlayerController->ClientTravel(TrimmedServerAddress, TRAVEL_Absolute);
	return true;
}

bool UMenuTravelBlueprintLibrary::JoinServerByAddressWithPlayerName(const UObject* WorldContextObject, const FString& ServerAddress, const FString& PlayerName)
{
	if (!WorldContextObject)
	{
		return false;
	}

	const FString TrimmedServerAddress = ServerAddress.TrimStartAndEnd();
	if (TrimmedServerAddress.IsEmpty())
	{
		return false;
	}

	UWorld* World = WorldContextObject->GetWorld();
	if (!World || World->bIsTearingDown)
	{
		return false;
	}

	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!PlayerController)
	{
		return false;
	}

	SaveDesiredPlayerName(WorldContextObject, PlayerName);
	PlayerController->ClientTravel(AppendTravelOption(TrimmedServerAddress, MakePlayerNameTravelOption(PlayerName)), TRAVEL_Absolute);
	return true;
}
