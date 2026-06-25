// Copyright Epic Games, Inc. All Rights Reserved.

#include "BlueprintFunctionLibraries/MenuTravelBlueprintLibrary.h"

#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

void UMenuTravelBlueprintLibrary::HostListenServer(const UObject* WorldContextObject, FName MapName)
{
	if (!WorldContextObject || MapName.IsNone())
	{
		return;
	}

	UGameplayStatics::OpenLevel(WorldContextObject, MapName, true, TEXT("listen"));
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
